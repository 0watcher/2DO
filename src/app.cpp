#include "app.hpp"

#include <chrono>
#include <thread>

namespace twodo
{
    Result<void> App::run()
    {
        while (m_is_running)
        {
            clearConsole();

            std::cout << "Main Menu:\n"
                << "[1] Tasks\n"
                << "[2] Users\n"
                << "[3] Settings\n"
                << "[0] Exit\n";

            auto main_option = get_valid_option();
            

            switch (main_option)
            {
            case 0:
                m_is_running = false;
                break;
            case 1:
                m_in_task_menu = true;
                while (m_in_task_menu)
                {
                    clearConsole();

                    std::cout << "Task Menu:\n"
                        << "[1] Task List\n"
                        << "[2] Add new task\n"
                        << "[3] Delete task\n"
                        << "[4] Modify task\n"
                        << "[5] Add comments\n"
                        << "[0] Back\n";

                    auto task_option = get_valid_option();

                    switch (task_option)
                    {
                    case 0:
                        m_in_task_menu = false;
                        break;
                    case 1:
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4:
                        break;
                    case 5:
                        break;
                    default:
                        std::cout << "Invalid option. Try again!\n";
                        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                        break;
                    }
                }
                break;
            case 2:
                std::cout << "users";
                break;
            case 3:
                std::cout << "settings";
                break;
            default:
                std::cout << "Invalid option. Try again!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                break;
            }
        }
        return Result<void>();
    }

    void App::clearConsole()
    {
        #ifdef _WIN32
                system("cls");
        #else
                system("clear");
        #endif
    }

    int App::get_valid_option()
    {
        while(true)
        {
            auto valid_value = []() -> Result<int> {
            int value = 0;
            std::cout << "=>";
            std::cin >> value;
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(INT_MAX, '\n');
                return Result<int>{.m_err = ErrorCode::invalid_input };
            }
            std::cin.ignore(INT_MAX, '\n');
            return Result<int>{.m_value = value};
            }();

            if (valid_value.m_err == ErrorCode::invalid_input)
            {
                std::cout << "Invalid input. Please enter a number!\n";
            }
            else
            {
                return valid_value.m_value.value();   
            }
        }
    }
}
