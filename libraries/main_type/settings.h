#ifndef SETTINGS_H
#define SETTINGS_H

// Вывод отладочных сообщений
//#define DEBUG_MODE 1

#define SERIAL_RATE 115200 // скорость последовательного порта Serial

// DTMF команды. Цифры от 0 до 255
enum {
	GUARD_ON = 1,		 // 1# - постановка на охрану
	GUARD_OFF,			 // 2# - снятие с охраны	
	GUARD_INFO,			 // 3# - информация о флаге охраны: 0 или 1
	TEL_ON_OFF,			 // 4# - включить/отключить звонок при тревоге
	SENS_ON_OFF,	     // 5# - включение/отключение отдельных датчиков
	BAT_CHARGE, 		 // 6# - показывает заряд батареи
	ADMIN_NUMBER_DEL,	 // 7# - админ больше не админ
	NUMBER_DEL_BY_INDEX, // 8# - удаляем номер из симкарты по индексу
	SM_CLEAR,		 	 // 9# - удалить все номера с симкарты
	EMAIL_ADMIN_PHONE,	 // 10# - отправляем на почту номер админа
	EMAIL_PHONE_BOOK,	 // 11# - отправка на почту телефонной книги
	MODEM_RESET			 // 12# - перезагрузка модема
};

// Датчики (не нужные модули закоментировать) 
 	
#define SENSOR_MOVE_ENABLE   	1// датчик движения
#define SENSOR_RADAR_ENABLE  	1// радар
#define SENSOR_FIRE_ENABLE 		1// датчик огня
#define SENSOR_GAS_ENABLE    	1// датчик дыма
#define SENSOR_DHT_ENABLE		1// датчик температуры и влажности
#define BEEP_ENABLE				1// пищалка

//*****************************************************************
//////////////////////////////////////////////////////////
/// Изменить параметры под себя
//////////////////////////////////////////////////////////
// Отправка почты

#define SMTP_SERVER 				F("\"smtp-devices.yandex.com\",25") // почтовый сервер яндекс и порт
#define SMTP_USER_NAME_AND_PASSWORD F("\"login\",\"password\"") // Лоргин и пароль от почты
#define SENDER_ADDRESS_AND_NAME 	F("\"login@yandex.com\",\"SIM800L\"")
#define RCPT_ADDRESS_AND_NAME 		F("\"login@mail.ru\",\"Ivan\"") // Адрес и имя получателя
//#define RCPT_CC_ADDRESS_AND_NAME 	F("\"login@yandex.com\",\"Ivan\"") // Адрес и имя получателя (копия)
//#define RCPT_BB_ADDRESS_AND_NAME 	F("\"login2@yandex.com\",\"Ivan\"") // Адрес и имя получателя (вторая копия)

//////////////////////////////////////////////////////////
// Задание пинов для датчиков

enum pins
	{
		RING_PIN = 2,	//2
		POWER_PIN,		//3
		DOOR_PIN,		//4 датчик двери (геркон). Один конец на GND, второй на цифровой пин arduino.

#ifdef SENSOR_MOVE_ENABLE			
		MOVE_PIN,    	//5 все датчики движения вешаем на один пин
#endif

#ifdef SENSOR_RADAR_ENABLE
		RADAR_PIN,   	//6 RCWL-0516
#endif

#ifdef SENSOR_FIRE_ENABLE
		FIRE_PIN,		//7
#endif

#ifdef SENSOR_DHT_ENABLE
		DHT_PIN,   		//8 датчик температуры и влажности DHT11
#endif

#ifdef BEEP_ENABLE
		BEEP_PIN,		//9
#endif
		BOOT_PIN		//10
	};	
	
#define GAS_PIN 	A0 // датчик газа (аналоговый вход)

#define SENS_COUNT  6
#define RESET_COUNT 3 // Сколько раз модем может не ответить до перезагрузки

/*
 * Интервалы между опросами датчиков
 */

#define ANALOG_PIN_OPROS_INTERVAL	120 // интервал опроса аналоговых пинов датчиков
#define ANALOG_SIGN_ALARM_VALUE 	200 // минимальное значение показаний аналоговых датчиков для тревоги
#define DHT_SIGN_ALARM_VALUE		35 	// минимальное значение показаний датчика температуры
#define DIGITAL_PIN_OPROS_INTERVAL	1   // интервал опроса цифровых пинов датчиков	
#define ALARM_MAX_TIME				60	// продолжительность тревоги sec, после чего счётчики срабатываний обнуляются

#endif // SETTINGS_H