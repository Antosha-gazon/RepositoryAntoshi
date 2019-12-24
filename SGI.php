<?php
$msg = trim(fgets(STDIN));
$first_delimiter = strpos($msg, "|");
$second_delimiter = strpos($msg, "|", $first_delimiter+1);
$temp = getenv("path");
$path = $temp.=substr($msg, 0, $first_delimiter);// берем путь из переменной окружения и имя файла из переданного сообщения
$type = substr($msg, $first_delimiter + 1, $second_delimiter - $first_delimiter - 1); // узнаем тип файла из переданного сообщения
$req = substr($msg, $second_delimiter + 1); // узнаем тело запроса из переданного сообщения

$elem_quantity = substr_count($req, '&'); // узнаем сколько элементов нужно положить в массив
// заполняем суперглобальный массив
for($i = 0; $i <= $elem_quantity; $i++)
{
  $first_marker = strpos($req, "=");
  $second_marker = strpos($req, "&");

  $key = substr($req, 0, $first_marker);
  if ($i != $elem_quantity)
    $value = substr($req, $first_marker + 1, $second_marker - $first_marker -1);
  else
    $value = substr($req, $first_marker + 1, strlen($req) - $first_marker - 1);
  $req = substr($req, $second_marker + 1);

//если символ зашифрован в url, декодируем в обычный вид
  $value = urldecode($value);

  $key = urldecode($key);


  if ($type == "POST")
    $_POST[$key] = $value;
  else
    $_GET[$key] = $value;
}
// вызываем требуемый скрипт
include $path;
?>
