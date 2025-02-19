# Задание 5. Сетевое программирование, сериализация, XML-файлы

[![Testing](https://github.com/ptrvsrg/chat/actions/workflows/maven.yml/badge.svg)](https://github.com/ptrvsrg/chat/actions/workflows/maven.yml)

## Описание
Напишите программу для общения через `Internet`. 
Программа должна состоять из двух частей: сервер и клиент. 
Сервер стартует в качестве отдельного приложения на определенном порту (задано в конфигурации). 
Клиент в виде приложения на `Swing` присоединяется к серверу по имени сервера и номеру порта.

## Минимальные возможности чата

+ Каждый участник чата имеет собственный никнейм, который указывается при присоединении к серверу.
+ Можно посмотреть список участников чата.
+ Можно послать сообщение в чат (всем участникам).
+ Клиент показывает все сообщения, которые отправили в чат с момента подключения. 
Список сообщений обновляется online.
+ Клиент отображает некоторые события: 
  + Подключение нового человека в чат;
  + Уход человека из чата и отключения клиента от чата по таймауту;
+ Сервер должен логировать все события, которые происходят на его стороне.
+ Чат работает через `TCP/IP` протокол.

## Форматы сообщений клиента и сервера 

### Java-объекты

Используется сериализация/десериализация Java-объектов для посылки/приема сообщений.

### XML-файлы 

Используются XML-сообщения.
Клиент и сервер должны поддерживать стандартный протокол (Это необходимо для возможности общение между клиентами, созданными разными учениками).
Вначале XML сообщения идут 4 байта (Java int) с его длиной. То есть сначала
читаются первые 4 байта и узнается длина оставшегося сообщения (в байтах).
Затем считывается само сообщение и далее обрабатывается как XML документ.

#### Описание протокола

Протокол взаимодействия для XML-сообщений (расширения приветствуются):

1. Регистрация

   + Client request

   ```xml
      <request id="MESSAGE_ID" name="login">
         <user>USER_NAME</user>
      </request>
   ```

   + Server error response

   ```xml
      <response id="MESSAGE_ID" name="error">
         <request>REQUEST_ID</request>
         <message>REASON</message>
      </response>
   ```

   + Server success response

   ```xml
      <response id="MESSAGE_ID" name="success">
         <request>REQUEST_ID</request>
      </response>
   ```

2. Запрос списка пользователей чата

   + Client request

   ```xml
      <request id="MESSAGE_ID" name="user_list">
      </request>
   ```

   + Server error response

   ```xml
      <response id="MESSAGE_ID" name="error">
         <request>REQUEST_ID</request>
         <message>REASON</message>
      </response>
   ```

   + Server success response

   ```xml
        <response id="MESSAGE_ID" name="success">
          <request>REQUEST_ID</request>
          <users>
            <user>
              <name>USER_1</name>
            </user>
            …
            <user>
              <name>USER_N</name>
            </user>
          </users>
        </response>
     ```

3. Отправка сообщения

   + Client request

   ```xml
     <request id="MESSAGE_ID" name="new_message">
         <message>MESSAGE</message>
     </request>
     ```

   + Server error response
   
   ```xml
      <response id="MESSAGE_ID" name="error">
         <request>REQUEST_ID</request>
         <message>REASON</message>
      </response>
   ```

   + Server success response
 
   ```xml
       <response id="MESSAGE_ID" name="success">
         <request>REQUEST_ID</request>
       </response>
    ```

4. Отключение

   + Client request
   
   ```xml
      <request id="MESSAGE_ID" name="logout">
      </request>
   ```
   
   + Server error response
   
   ```xml
      <response id="MESSAGE_ID" name="error">
         <request_id>REQUEST_ID</request_id>
         <message>REASON</message>
      </response>
   ```
   
   + Server success response
   
   ```xml
       <response id="MESSAGE_ID" name="success">
         <request_id>REQUEST_ID</request_id>
       </response>
   ```

5. Новый пользователь

   + Server event

   ```xml
      <event id="MESSAGE_ID" name="login">
         <name>USER_NAME</name>
      </event>
   ```

6. Отправка сообщения другим пользователем

   + Server event

   ```xml
      <event id="MESSAGE_ID" name="new_message">
         <name>USER_NAME</name>
         <message>MESSAGE</message>
      </event>
   ```

7. Другой пользователь отключился

   + Server event

   ```xml
      <event id="MESSAGE_ID" name="logout">
         <name>USER_NAME</name>
      </event>
   ```

## Рекомендации

+ Сервер слушает порт с помощью класса `java.net.ServerSocket`
+ Клиент подключается к серверу с помощью класса `java.net.Socket`
+ XML-сообщение читать с помощью `DOM parser`:
   ```
      DocumentBuilderFactory.newInstance().newDocumentBuilder().parse()
   ```

+ Сериализация/десериализация объекта выполняется через классы `ObjectInputStream` и `ObjectOutputStream`
