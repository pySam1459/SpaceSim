#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <thread>

#include "camera.hpp"
#include "constants.hpp"
#include "mesh.hpp"
#include "models.hpp"
#include "read_file_to_string.hpp"
#include "shaders.hpp"
#include "state.hpp"


GLFWwindow* create_window()
{
    if (!glfwInit())
        throw std::runtime_error("GLFW initilisation failed");
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "Space Sim", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW window");
    }

    return window;
}

void framebuffer_size_callback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLAPIENTRY debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam)
{
    std::cerr << "[GL DEBUG] " << message << std::endl;
    if (severity == GL_DEBUG_SEVERITY_HIGH)
        std::terminate();
}


namespace {
ShaderProgram load_basic_shader()
{
    auto vertexShader = read_file_to_string("shaders/basic/basic.vert"); 
    auto fragmentShader = read_file_to_string("shaders/basic/basic.frag"); 

    return make_program(
        compile_shader(GL_VERTEX_SHADER, vertexShader),
        compile_shader(GL_FRAGMENT_SHADER, fragmentShader)
    );
}

State create_state()
{
    State state;
    state.transforms.push_back(Transform{{0, 0, -3.0f}, {1.0f, 0, 0, 0}, 1.0f});
    state.transforms.push_back(Transform{{5, 0, -3.0f}, {1.0f, 0.5f, 0, 0}, 0.5f});
    
    state.models.push_back(std::make_unique<Cube>(0));
    state.models.push_back(std::make_unique<Cube>(1));
    return state;
}
}

class Sim {
    const std::uint32_t tps;
    const std::chrono::nanoseconds target_frame_ns;
    const double fixed_dt;
    const std::uint32_t panic_update_cap;
    std::atomic_bool running{true};

    GLFWwindow* window;
    ShaderProgram shader_program;
    glm::mat4 proj_mat;

    State state;
    Camera cam{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};

public:
    explicit Sim(GLFWwindow* window,
                 std::uint32_t tps = 60,
                 std::uint32_t target_fps = 144,
                 std::uint32_t max_updates_per_fl = 5)
        : window(window),
          tps(tps),
          target_frame_ns{1'000'000'000ull / target_fps},
          fixed_dt{1.0 / static_cast<double>(tps)},
          panic_update_cap{max_updates_per_fl},
          shader_program{ load_basic_shader() },
          state{ create_state() }
    {
        cam.window_setup(window);

        proj_mat = glm::perspective(glm::radians(60.0f), float(kWidth)/kHeight, 0.1f, 100.0f);
    }

    ~Sim() {}

    void run()
    {
        using clock = std::chrono::steady_clock;

        auto previous_time  = clock::now();
        std::chrono::nanoseconds lag{0};

        std::uint32_t tick_counter = 0;
        std::uint32_t render_counter = 0;
        std::chrono::steady_clock::time_point last_stats_time = clock::now();

        bool has_ticked{false};
        // constexpr auto safety_margin = std::chrono::microseconds{500};

        while (running) {
            auto current_time = clock::now();
            auto elapsed = current_time - previous_time;
            previous_time = current_time;
            lag += elapsed;

            std::uint32_t updates_this_frame = 0;
            while (lag >= tick_interval() && updates_this_frame < panic_update_cap) {
                tick(static_cast<float>(fixed_dt));
                lag -= tick_interval();
                ++updates_this_frame;
                has_ticked = true;

                ++tick_counter;
            }

            // Spiral-of-death guard
            if (updates_this_frame >= panic_update_cap) {
                lag = std::chrono::nanoseconds{0}; // drop excess lag
            }

            if (has_ticked) {
                const double alpha = static_cast<double>(lag.count())
                                / static_cast<double>(tick_interval().count());
                render(static_cast<float>(alpha)); // alpha in [0,1)
                ++render_counter;
            }

            auto now = clock::now();
            if (now - last_stats_time >= std::chrono::seconds{1}) {
                std::cout << "TPS: " << tick_counter << " | FPS: " << render_counter << std::endl;
                tick_counter = 0;
                render_counter = 0;
                last_stats_time = now;
            }

            // FPS limiter
            // auto frame_deadline = current_time + target_frame_ns;
            // while (clock::now() + safety_margin < frame_deadline) {
            //     std::this_thread::sleep_for(std::chrono::microseconds{200});
            // }
        }
    }

    static void setup_window(GLFWwindow* window)
    {
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSwapInterval(1); // v-sync

        // z-buffer depth test
        glEnable(GL_DEPTH_TEST);

        // back-face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debug_callback, nullptr);
    }

    void stop() noexcept { running = false; }

private:
    void tick(float dt)
    {
        glfwPollEvents();
        if (glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            stop();
            return;
        }
        cam.keyInput(window, dt);

        state.swap();
        // physics
    }

    void render(float alpha)
    {
        if (!running) return;

        glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program.id);

        glm::mat4 view = cam.getView();
        glm::mat4 vp = proj_mat * view;

        for (const auto& model : state.models) {
            const Transform tf = interpolate(state.prev_tfs[model->idx], state.transforms[model->idx], alpha);
            const glm::mat4 mvp = vp * tf.to_model_mat4();
            shader_program.set_mvp(mvp);
            model->draw();
        }

        glfwSwapBuffers(window);
    }

    constexpr std::chrono::nanoseconds tick_interval() const
    {
        return std::chrono::nanoseconds(
                   static_cast<std::int64_t>(1'000'000'000ull / tps));
    }
};

int main()
try {
    GLFWwindow* window = create_window();
    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        glfwTerminate();
        throw std::runtime_error("gladLoadGL failed");
    }

    Sim::setup_window(window);

    Sim sim(window, 60, 144);
    sim.run();

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;

} catch (std::exception& e) {
    std::cerr << "[FATAL] main:" << e.what() << std::endl;
    return EXIT_FAILURE;
}
