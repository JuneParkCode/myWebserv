# webserv plus

# I/O Multiplexing + Multithread  웹서버

## 프로젝트를 시작한 계기

- 42에서 수행한 webserv 과제의 경우, single thread 기반의 I/O multiplexing web server 였습니다. 개인적으로 해당 과제를 하면서 아쉬웠던 점들이 많았습니다. 더 많은 연결, 더 많은 동시 처리, 더 강력한 성능, 더 깔끔한 코드로 작성해보고 싶다는 욕심이 들어 해당 프로젝트를 시작하게 되었습니다.

## 목표

- 해당 프로젝트의 가장 큰 목표는 최대한 많은 클라이언트를 동시에 접속하게 하는 것, 그리고 큰 파일들까지도 빠른 속도로 전송/수신 할 수 있는 서버를 구축하는 것입니다.
- 이 목표를 위해서 멀티 스레드를 활용하며, 그 동안 42에서 배워온 다양한 기술/이론을 적용해보는 프로젝트이기도 합니다.
- 이전 42 webserv 프로젝트에서는 충분히 I/O multiplexing 을 적용해보았기에 해당 프로젝트에서는 “어떻게 하면 멀티 스레드를 최대한 활용할 수 있을까?” 라는 질문과 멀티스레드를 사용하며 파생되는 문제들을 해결해나가는데에 주력했습니다.
- 또한,  짧은 시간안에 최대한 많은 요청을 처리하기 위해 어떻게 내부 로직을 최적화할 것인지에 대해 이해하는 것도 큰 목표입니다.
- nginx는 config 파일을 기반으로 돌지만, 해당 서버는 서버 프로그래밍이 직접 가능하도록 `.get("/img", (){ func... })` 과  같이 라우터에 해당된 작업을 지정할 수 있는 api를 지원하도록 만들고자합니다.


## 결과
- 야심차게 멀티스레딩으로 서버를 더 빠르게! 라고 시작했지만... 결과는 좋지 않았습니다. 해당 과정은 Issues 탭에 잘 기록해두었습니다.
