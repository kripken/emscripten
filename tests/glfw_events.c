#ifdef USE_GLFW
    #if USE_GLFW == 2
        #include <GL/glfw.h>
    #else
        #include <GLFW/glfw3.h>
    #endif
    #include <stdio.h>
    #include <emscripten.h>

    #define MULTILINE(...) #__VA_ARGS__
    #define WIDTH 640
    #define HEIGHT 480

    // Setup tests
    typedef struct {
        int mouse;
        double x, y;
        int button;
        int action;
        int modify;
    } test_args_t;

    typedef struct {
        char cmd[80];
        test_args_t args;
    } test_t;

    // Javascript event.button 0 = left, 1 = middle, 2 = right
    test_t g_tests[] = {
        { "Module.injectMouseEvent(10.0, 10.0, 'mousedown', 0)", { 1, 10.0, 10.0, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, -1 } },
        { "Module.injectMouseEvent(10.0, 20.0, 'mouseup', 0)", { 1, 10.0, 20.0, GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, -1 } },
        { "Module.injectMouseEvent(10.0, 30.0, 'mousedown', 1)", { 1, 10.0, 30.0, GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS, -1 } },
        { "Module.injectMouseEvent(10.0, 40.0, 'mouseup', 1)", { 1, 10.0, 40.0, GLFW_MOUSE_BUTTON_MIDDLE, GLFW_RELEASE, -1 } },
        { "Module.injectMouseEvent(10.0, 30.0, 'mousedown', 2)", { 1, 10.0, 30.0, GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS, -1 } },
        { "Module.injectMouseEvent(10.0, 40.0, 'mouseup', 2)", { 1, 10.0, 40.0, GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE, -1 } },
        //{ "Module.injectMouseEvent(10.0, 50.0, 'mousewheel', 0)", { 10.0, 50.0, -1, -1, -1 } },
        //{ "Module.injectMouseEvent(10.0, 60.0, 'mousemove', 0)", { 10.0, 60.0, -1, -1, -1 } }

        { "Module.injectKeyEvent('keydown', 8)", { 0, 0.0, 0.0, GLFW_KEY_BACKSPACE, GLFW_PRESS, -1 } },
        { "Module.injectKeyEvent('keyup', 8)", { 0, 0.0, 0.0, GLFW_KEY_BACKSPACE, GLFW_RELEASE, -1 } },
        { "Module.injectKeyEvent('keydown', 9)", { 0, 0.0, 0.0, GLFW_KEY_TAB, GLFW_PRESS, -1 } },
        { "Module.injectKeyEvent('keydown', 112)", { 0, 0.0, 0.0, GLFW_KEY_F1, GLFW_PRESS, -1 } },

        #if USE_GLFW == 2
            { "Module.injectKeyEvent('keydown', 27)", { 0, 0.0, 0.0, GLFW_KEY_ESC, GLFW_PRESS, -1 } },
        #else
            { "Module.injectKeyEvent('keydown', 27)", { 0, 0.0, 0.0, GLFW_KEY_ESCAPE, GLFW_PRESS, -1 } },
        #endif
    };

    static unsigned int g_test_actual = 0;
    static unsigned int g_test_count = sizeof(g_tests) / sizeof(test_t);
    static unsigned int g_state = 0;

    #if USE_GLFW == 2
        static void on_mouse_button_callback(int button, int action)
    #else
        static void on_mouse_button_callback(GLFWwindow* window, int button, int action, int modify)
    #endif
    {
        test_args_t args = g_tests[g_test_actual].args;
        if (args.button == button && args.action == action)
        {
            g_state |= 1 << g_test_actual;
        }
        else
        {
            printf("Test %d: FAIL\n", g_test_actual);
        }
    }

    #if USE_GLFW == 2
        static void on_mouse_move(int x, int y)
    #else
        static void on_mouse_move(GLFWwindow* window, double x, double y)
    #endif
    {
        test_args_t args = g_tests[g_test_actual].args;
        if (args.x == x && args.y == y)
        {
            g_state |= 1 << g_test_actual;
        }
        else
        {
            printf("Test %d: FAIL\n", g_test_actual);
        }
    }

    #if USE_GLFW == 2
        static void on_key_callback(int key, int action)
    #else
        static void on_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    #endif
    {
        test_args_t args = g_tests[g_test_actual].args;
        if (args.button == key && args.action == action)
        {
            g_state |= 1 << g_test_actual;
        }
        else
        {
            printf("Test %d: FAIL\n", g_test_actual);
        }
    }

    #if USE_GLFW == 3
        static void on_mouse_wheel(GLFWwindow* window, double x, double y)
        {
            test_args_t args = g_tests[g_test_actual].args;
            if (args.x == x && args.y == y)
            {
                g_state |= 1 << g_test_actual;
            }
            else
            {
                printf("Test %d: FAIL\n", g_test_actual);
            }
        }

        static void on_error(int error, const char *msg)
        {
            printf("%d: %s\n", error, msg);
        }
    #endif

    int main()
    {
        int result = 0;
        unsigned int success = 0;

        emscripten_run_script(MULTILINE(
            Module.injectMouseEvent = function(x, y, event_, button) {
                var canvas = Module['canvas'];
                var event = new MouseEvent(event_, {
                    'view': window,
                    'bubbles': true,
                    'cancelable': true,
                    'screenX': canvas.offsetLeft + x,
                    'screenY': canvas.offsetTop + y,
                    'clientX': canvas.offsetLeft + x,
                    'clientY': canvas.offsetTop + y,
                    'button': button
                });
                canvas.dispatchEvent(event);

                //var event = document.createEvent("MouseEvents");
                //var canvas = Module['canvas'];
                //event.initMouseEvent(event_, true, true, window, 0, canvas.offsetLeft + x, canvas.offsetTop + y, canvas.offsetLeft + x, canvas.offsetTop + y, 0, 0, 0, 0, button, null);
                //canvas.dispatchEvent(event);
            };

            Module.injectKeyEvent = function(type, keyCode) {
                var keyboardEvent = document.createEvent("KeyboardEvent");
                var initMethod = typeof keyboardEvent.initKeyboardEvent !== 'undefined' ? "initKeyboardEvent" : "initKeyEvent";
                keyboardEvent[initMethod](type, true, true, window, false, false, false, false, keyCode, 0);
                canvas.dispatchEvent(keyboardEvent);
            };
        ));
        
        glfwInit();

        #if USE_GLFW == 2
            glfwOpenWindow(WIDTH, HEIGHT, 5, 6, 5, 0, 0, 0, GLFW_WINDOW); // != GL_TRUE)
            
            glfwSetMousePosCallback(on_mouse_move);
            glfwSetMouseButtonCallback(on_mouse_button_callback);
            glfwSetKeyCallback(on_key_callback);
            //glfwSetCharCallback(...);
        #else
            glfwSetErrorCallback(on_error);
            printf("%s\n", glfwGetVersionString());

            GLFWwindow * _mainWindow = NULL;
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            _mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "glfw3_events", NULL, NULL);
            glfwMakeContextCurrent(_mainWindow);

            glfwSetMouseButtonCallback(_mainWindow, on_mouse_button_callback);
            glfwSetCursorPosCallback(_mainWindow, on_mouse_move);
            glfwSetScrollCallback(_mainWindow, on_mouse_wheel);
            glfwSetKeyCallback(_mainWindow, on_key_callback);
            //glfwSetCharCallback(_mainWindow, ...);
        #endif

        for (int i = 0; i < g_test_count; ++i)
        {
            g_test_actual = i;
            test_t test = g_tests[g_test_actual];
            emscripten_run_script(test.cmd);

            if (test.args.mouse) {
            #if USE_GLFW == 2
                if (glfwGetMouseButton(test.args.button) != test.args.action)
            #else
                if (glfwGetMouseButton(_mainWindow, test.args.button) != test.args.action)
            #endif
                {
                    printf("Test %d: FAIL\n", g_test_actual);
                    g_state &= ~(1 << g_test_actual);
                }
            } else {
                // Keyboard.
            #if USE_GLFW == 2
                if (glfwGetKey(test.args.button) != test.args.action)
            #else
                if (glfwGetKey(_mainWindow, test.args.button) != test.args.action)
            #endif
                {
                    printf("Test %d: FAIL\n", g_test_actual);
                    g_state &= ~(1 << g_test_actual);
                }
            }
        }

        glfwTerminate();

        success = (1 << (sizeof(g_tests) / sizeof(test_t))) - 1; // (2^count)-1

    #ifdef REPORT_RESULT
        result = g_state == success;
        REPORT_RESULT();
    #else
        printf("%d == %d = %d", g_state, success, g_state == success);
    #endif

        return 0;
    }
#endif