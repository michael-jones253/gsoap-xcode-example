# gsoap-xcode-example
Xcode client and server example for gsoap soap c++ SOAP library.

Built with gsoap 2.8.

https://www.ohloh.net/p/gsoap

## Target mj_calc_cgi
This is a cgi executable, which can either be dropped in the CGI bin directory or tested as a standalone single threaded server.

## Target mj_calc_server
This is a multi threaded executable. It uses some c11 features such as std::async, however the gsoap library is c11. I used std::forward_list only out of curiosity because it is supposed to be the most memory efficient STL list and because I wanted to see its use case limitations.

## Target mj_calc_xcode
This is a client with command line arguments to allow testing to either a local standalone server, CGI, apache module (mod_gsoap) and a remote calc website provided by the open source gsoap developer.

## Building GSOAP Apache modules.

CGI is reputed to be slow - I guess because it is effectively two programs talking to each other over stdin and stdout pipes, so an apache module (mod_gsoap) and custom server dynamic library to be loaded by this module is recommended for performance. This is not part of the xcode project because it must be built using an apache build tool called apxs.

My OS X Yosemite came with the apxs and the other Apache build tools it uses e.g. apr-1-config, however the resulting module and dylib crashed. So I ended up building apache server with debug options, and generating a new set of apache module build tools so I was guaranteed to be using the same header files for the httpd and mod_gsoap library. Apache httpd can be started with the -X option for running under lldb (run in forground and only one worker).

