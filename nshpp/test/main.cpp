#include <nshpp/StaticBinding.hpp>
#include <nshpp/Stdio.hpp>
#include <nshpp/nsh.hpp>

// namespace my_shell {

// struct my_context
// {
//     std::size_t some_data;
// };

// using my_handler = int (*)(my_context&, int, char**);
// using my_cmd = nsh::Cmd<my_handler>;

// constexpr nsh::BaseSettings<my_handler> my_settings {
//     .LineBufferSize = 16,
//     .MaxArgumentCount = 16,
//     .CmdNameSize = 16,
//     .CmdCount = 16,
//     .HistorySize = 16,
// };

// constexpr my_cmd help("help", [](my_context&, int, char**) noexcept {
//     puts("This is an helpful message!");
//     return 42;
// });

// constexpr my_cmd exit("exit", [](my_context&, int, char**) noexcept {
//     puts("Exiting!");
//     return 69;
// });

// constexpr my_cmd toto("toto", [](my_context&, int, char**) noexcept {
//     puts("Toto!");
//     return 99;
// });

// constexpr my_cmd tata("tata", [](my_context&, int, char**) noexcept {
//     puts("Tata!");
//     return 99;
// });

// constexpr nsh::CmdList cmds(help, exit, toto, tata);

// } // namespace my_shell

// void autocomplete(auto shell, auto cmds, auto ctx, auto argc, auto argv, std::string_view str)
// {
//     using Autocompleter = decltype(shell.addons.autocompleter);
//     constexpr bool has_autocomplete = !std::is_same_v<Autocompleter, nsh::NoAutocompleter>;
//     if constexpr (has_autocomplete) {
//         constexpr auto matching = shell.addons.autocompleter.autocomplete(cmds, str);
//         for (const auto& cmd : matching) {
//             cmd.handler(ctx, argc, argv);
//         }
//     }
// }

// void suggest(auto shell, auto cmds, std::string_view str)
// {
//     using Suggester = decltype(shell.addons.suggester);
//     constexpr bool has_suggest = !std::is_same_v<Suggester, nsh::NoSuggester>;
//     if constexpr (has_suggest) {
//         constexpr auto suggestion = shell.addons.suggester.suggest(cmds, str);
//         puts(suggestion.data());
//     }
// }

void handle_0() noexcept
{
    puts(__func__);
}

void handle_1() noexcept
{
    puts(__func__);
}

void handle_23()
{
    puts(__func__);
}

constexpr auto handle_4 = [] { puts(__func__); };

void handle_eof() noexcept
{
    puts(__func__);
}

void handle_other() noexcept
{
    puts(__func__);
}

void invalid_handler(int)
{
}

using MyNshBindings = nsh::StaticBinding<
    nsh::BindByDefault<handle_other>,
    nsh::Bind<0, handle_0>,
    nsh::Bind<1, handle_1>,
    nsh::Bind<2, handle_23>,
    nsh::Bind<3, handle_23>,
    nsh::Bind<4, handle_4>,
    nsh::Bind<EOF, handle_eof>>;

namespace nsh::tools {

int main(int, char**)
{
    Nsh<nsh::StdIo, MyNshBindings> nsh;
    nsh.readline();
    return 0;
}

} // namespace nsh::tools
