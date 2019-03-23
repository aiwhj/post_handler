post handler
=====
Provide a hook before receiving the body.
Pre-process the header before receiving the body.

## install
```shell
git clone https://github.com/aiwhj/post_handler.git
cd post_handler
phpize
configure
make && make install
```

## config
```ini
[post_handler]
;configuration for php post_handler module
extension = post_handler.so

;whether to open 
;1-Y 0-N(Default)
post_handler.enable = 0

;handler type
;1-file(Default) 2-Command Line
post_handler.handler_type = 1

;The full path to the file or the command line
post_handler.handler = /work/post_handler/handler.php
;post_handler.handler="if (!isset($_SERVER['HTTP_HANDLER']) || $_SERVER['HTTP_HANDLER'] != 'post') { echo 'do not receive the body' . PHP_EOL; exit(1); }"

```
## notice
1. works only when `Content-Type = multipart/form-data`
2. the `exit status` must be Integer. 
3. when `exit status` is greater than 0, the `post handler` stops receiving the body and stops executing your real code. 
4. you have to manually print some strings to tell the `Web Server`, otherwise `HTTP Code` will be `502`.

## example
```php
<?php
/**
 * handler.php
 * 
 * When the HTTP_HANDLER in the header is not equal to the 'post', do not receive the body.
 * Global variables only $_SERVER
 */

if (!isset($_SERVER['HTTP_HANDLER']) || $_SERVER['HTTP_HANDLER'] != 'post') {
    echo 'do not receive the body' . PHP_EOL;
    exit(1);
}

```
