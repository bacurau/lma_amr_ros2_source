#include <iostream>
#include <filesystem>

void main_2(void);

int main()
{
    auto cwd = std::filesystem::current_path();
    std::cout << "Hello world! " << std::endl;
    std::cout << "I'm in: " << cwd << std::endl;
    main_2();


    return 0;
}