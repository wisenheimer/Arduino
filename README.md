# Arduino
signaling Arduino+SIM800L

https://pikabu.ru/story/proekt_gsmgprs_signalizatsii_na_arduino_6327233

https://pikabu.ru/story/proekt_gsmgprs_signalizatsii_na_arduino_ch2_6362226

https://pikabu.ru/story/proekt_gsmgprs_signalizatsii_na_arduino_ch3_6409085

Это старая версия прошивки.
Новая прошивка с поддержкой смс на русском языке находится здесь
https://github.com/wisenheimer/Signaling-Blynk

Для работы со старой схемой требуется привести в соответствие номера зарезервированных пинов

#define	BOOT_PIN  5 // перезагрузка модема
#define RADAR_PIN 6 // микроволновый датчик движения RCWL-0516

здесь https://github.com/wisenheimer/Signaling-Blynk/blob/master/libraries/main_type/main_type.h
