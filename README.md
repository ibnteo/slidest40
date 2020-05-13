# Touch slide keyboard Slidest-40

![](Layout/layout.png)

Printing is done by touching or holding your finger through the touch areas. For example, to print the letter O, you need to touch area 1, H - hold from area 1 to 3 (slide `13`), W - `124`, the number 1 is `1243`.

...

Code: [Arduino](Arduino/)

Controller: Pro-Micro and Adafruit-MPR121

Library: https://github.com/adafruit/Adafruit_MPR121

Copy `Adafruit_MPR121.h` and `Adafruit_MPR121.cpp` files to `Arduino` directory.

---

# Сенсорная слайдовая клавиатура Слайдость-40

Печать производится прикосновением или проведением пальца через сенсорные области. Например, для печати буквы О необходимо прикоснуться к области 1, В - провести из области 1 к 3 (слайд `13`), Ш - `124`, цифра 1 это `1243`, восклицательный знак ! - `12431`.

Вот ещё некоторые аккроды:
Переключение раскладок русская/английская - `121`  
Backspace - `21`  
Ctrl+Backspace - `212`  
Стрелки: вверх - `64`, вниз - `46`, влево - `65`, вправо - `56`  
Перемещение по словам: Ctrl+Left - `656`, Ctrl+Right - `565`  
PageUp - `646`, PageDown - `464`, Tab - `34`

Режим Alt+Tab - `346`, после чего +Tab - `34` или +Shift+Tab - `43`, и +Left - `65` или +Right - `56` (в разных оконных оболочках переключение внутри Alt+Tab осуществляется по-разному), выход из режима - клик на любое поле.

Модификаторы: Shift - `42`, Ctrl - `343`, Alt - `424`, Win - `4246`, отпустить нажатые модификаторы - `4242`, можно и пробелом - `4`, при печати любого символа модификаторы отпускаются.

Слой функциональных кнопок - `1213`, выход из слоя либо кликом на любом поле, либо вводом слайда из этого слоя. Слайды четверные как цифры, либо тройные, короче цифр, например, F1 - `124` или `1243`, и так до F12 - `465` или `4653`, затем идут Caps Lock, Scroll Lock, Print Screee, Pause/Break.  
Находясь в слое функциональных кнопок можно выбрать операционную систему, Linux - `3135`, Windows - `3431`, MacOS - `3531`, Android - `3435`, различие в переключении раскладок, Shift+Alt, Shift+Ctrl, Cmd+Space или Win+Space (это одно и то же), и в MacOS-режиме меняются местами модификаторы Ctrl и Cmd (Win).

Слой цифрового блока - `12135`, выход таким же слайдом. Слайды четверные как цифры, либо тройные короче цифр. Распределение кнопок по слою следующее: 1234567890, Num Lock, точка или запятая (зависит от выбранной раскладки), -+/*

Знаки печатаются в любой раскладке, если они в текущей отсутвуют, то произойдёт переключение в противопроложную раскладку, печать знака, и переключение обратно. То есть, из русской раскладки можно печатать отсутствующие в ней символы `~@#$^&[]{}<>|, а из английской можно напечатать знак №.

Слайды навигации задействуют _принцип умножителя_, если на противоположной половине клавиатуры зажать область, а на текущей ввести управляющий слайд, то он будет нажат указанное число раз по номеру зоны, вместо единицы будет нажат Ctrl. Например, чтобы удалить три символа, можно прикоснуться к области `3`, а на другой половине провести слайд Backspace - `21`, а для перемещения по словам можно использовать не слайды с Ctrl (например, на слово влево Ctrl+Left - `656`), а зажать `1` и вводить слайд `65` сколько нужно раз.

Код: [Arduino](Arduino/)

Контроллер: Pro-Micro и Adafruit-MPR121

Библиотека: https://github.com/adafruit/Adafruit_MPR121

Скопируйте файлы `Adafruit_MPR121.h` и `Adafruit_MPR121.cpp` в директорию `Arduino`.
