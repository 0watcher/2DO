#include "app.hpp"
#include <chrono>
#include <thread>

#ifdef _WIN32
#define CLEAR_TERM() system("cls")
#else
#define CLEAR_TERM() system("clear")
#endif


namespace twodo
{
Result<None, RunError> App::run()
{
    while (m_is_running)
    {
        CLEAR_TERM();

        std::cout << "Main Menu:\n"
                  << "[1] Tasks\n"
                  << "[2] Users\n"
                  << "[3] Settings\n"
                  << "[0] Exit\n"
                  << "->";

        auto main_option = get_valid_option();

        if (main_option)
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
                        CLEAR_TERM();

                        std::cout << "Task Menu:\n"
                                  << "[1] Task List\n"
                                  << "[2] Add new task\n"
                                  << "[3] Delete task\n"
                                  << "[4] Modify task\n"
                                  << "[5] Add comments\n"
                                  << "[0] Back\n"
                                  << "->";

                        auto task_option = get_valid_option();

                        if (task_option)
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
                                    sleep(2000);
                                    break;
                            }
                        }
                        else
                        {
                            std::cerr << "Invalid input. Try again!\n";
                            sleep(2000);
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
                    sleep(2000);
                    break;
            }
        }
        else
        {
            std::cerr << "Invalid input. Try again!\n";
            sleep(2000);
        }
    }
    return Ok<None, RunError>({});
}

Result<int, InputError> App::get_valid_option() noexcept
{
    int value = 0;
    std::cin >> value;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(INT_MAX, '\n');
        
        return Err<int, InputError>(InputError::InvalidInput);
    }
    std::cin.ignore(INT_MAX, '\n');
    return Ok<int, InputError>(std::move(value));
}
}  // namespace twodo