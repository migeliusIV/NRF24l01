# NRF24l01
Если говорить по-простецки, в папке "роботы 2023" всё подробно разжёвано для новичков. В остальных для полных новичков.
Файлы test_priem и test_peredacha нужны только для того, чтобы изучить самые азы, понять, как устроена программа связи двух модулей.
Файлы norm_priem и norm_peredacha использовались мною, чтобы настроить массив передаваемых данных: два аналоговых значения и одна кнопка, так было сначала. С их помощью я управлял яркостью и включением/выключением светодиодов, результат работы программы продемонстрирован на видео priem-peredaca-video.zip.
"Скетч приемника..." и "Скетч передатчика..." нужны для проерки дальности работы модулей, чтобы уменьшить кол-во недошедших файлов.
Алгоритм поиска свободных каналов, он же algorithm_of_searching_a_pipe используется в двух случаях. Во-первых, когда необходимо проверить работу приемо-передатчика в целом. Для этого программа устанавливается на микроконт., а к нему в свою очередь подключается nrf24. И если программ в монитор порта выводит два ряда 0-7, 0-f, а остальные от нуля до каких-то значений, то всё хорошо. А если программа выглядит, как сплошные иероглифы, то nrf-ке кабзда. Второе применение этой программы заключается в возможности просмотра "чистых" и "шумящих" каналов частоты. Если в столбце много нулей, то такой канал называется "чистым", а чем выше значение в выводимых чисел в столбце и рядом, тем проблемнее будет работа на таком канале. Почитайте подробнее об этом.
БОльшая часть промышленных и заводских устройств работают на частоте nrf: 2,4ГГц, но они работают на каналах до 60. Поэтому все роботы должны быть запрограммированы на частоты > 60 (> 2,460ГГц).
"Программа диназавров" написана ребятами в 2021, она значительно короче полседних версий, но написана людьми, знающими с++, так что последний вариант более понятный для простых смертных. Но если есть желание, то можете переписать, это было бы круто.
Разнесите всех в 2023. У вас есть все шансы)
Бородавко Никита.
Сорокин Михаил.
