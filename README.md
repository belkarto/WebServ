# Ray_Tracer [![GitHub stars](https://img.shields.io/github/stars/belkarto/WebServ?label=Star%20Project&style=social)](https://github.com/belkarto/WebServ/stargazers)

<p align="center">
	<img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/belkarto/WebServ?color=lightblue" />
	<img alt="Code language count" src="https://img.shields.io/github/languages/count/belkarto/WebServ?color=yellow" />
	<img alt="GitHub top language" src="https://img.shields.io/github/languages/top/belkarto/WebServ?color=blue" />
	<img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/belkarto/WebServ?color=green" />
	<img alt="GitHub last commit" src="https://img.shields.io/github/license/belkarto/WebServ?color=green" />
</p>

![github-header-image(1)](https://github.com/ilhamsalhi/WebServ/assets/118683350/4d3083f4-f952-42f5-82b2-b7ec18161b31)

## Description ✨🌐
A web service is a software program that offers specific functionalities over the internet, like a self-service machine accessible by other programs.
### Functionality
Acts as an intermediary between clients and web server

Listens for incoming HTTP requests from clients, interprets them, and takes appropriate actions.

## Features ✨
**Network and Protocol Support 🌐**

* 💬🔗 TCP/IP: Ensures reliable communication across networks.
* 🔄🔀 epoll (or similar): Enables efficient handling of multiple concurrent connections, improving scalability and performance.

**HTTP Methods 📡**

* 📨 GET: Retrieves resources from the server.
* 📝 POST: Submits data to the server for creation or updates.
* 🗑️ DELETE: Requests removal of resources from the server.

**Dynamic Content Generation 💡**

*⚙️ CGI: Integrates with external programs for dynamic content generation, providing flexibility for interactive web pages.

**Static Content Serving📦**

*✔️ Delivers static resources (HTML, images, CSS, JavaScript) efficiently.


## Setup 🛠️
### Usage 🚀
Run the following to make the project
```
make
```
After making the project you can then run the program as follow
```
./webserv <optional config file>
```

## Project layout 📁
    ├─ Makefile    			
    ├─ include/
    ├─ lib/
    ├─ config/      	
    ├─ src/
    └─ LICESNE

## Resources 📚
[🔗 Simple http server](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)

[🔗 Simple server using C++](https://ncona.com/2019/04/building-a-simple-server-with-cpp/)

[🔗 Web programing](https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm)

[🔗 None bloking I/O](https://www.ibm.com/docs/en/i/7.3?topic=designs-example-nonblocking-io-select)

[🔗 Nginx http core conf](https://nginx.org/en/docs/http/ngx_http_core_module.html?&_ga=2.231087193.711261974.1697311162-1584719497.1696689803#error_page)

## License 📃

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
