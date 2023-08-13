#include "app.hpp"

#include <chrono>
#include <thread>

namespace twodo
{
    Result<None, RunError> App::run()
    {
        while (m_is_running)
        {
            clearConsole();

            std::cout << "Main Menu:\n"
                      << "[1] Tasks\n"
                      << "[2] Users\n"
                      << "[3] Settings\n"
                      << "[0] Exit\n"
                      << "->";

            auto main_option = get_valid_option();

            if (main_option.error() == InputError::InvalidInput)
            {
                switch (main_option.value())
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
                                  << "[0] Back\n"
                                  << "->";

                        auto task_option = get_valid_option();

                        if (task_option.error() == InputError::InvalidInput)
                        {
                            switch (task_option.value())
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
                                std::cerr << "Invalid option. Try again!\n";
                                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                                break;
                            }
                        }
                        else
                        {
                            std::cerr << "Invalid input. Try again!\n";
                            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
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
                    std::cerr << "Invalid option. Try again!\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                    break;
                }
            }
            else
            {
                std::cerr << "Invalid input. Try again!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }
        }
        return Ok<RunError>();
    }

    void App::clearConsole() noexcept
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    Result<int, InputError> App::get_valid_option()
    {
        int value = 0;
        std::cin >> value;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            return Error<int, InputError>(InputError::InvalidInput);
        }
        std::cin.ignore(INT_MAX, '\n');
        return Ok<int, InputError>(value);
    }
}
