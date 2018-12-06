#ifndef SETTINGS_H
#define SETTINGS_H

/*
	Данный файл настраивается пользователем под свои данные и датчики
*/

// Вывод отладочных сообщений
//#define DEBUG_MODE 1

#define SERIAL_RATE 115200 // скорость последовательного порта Serial

// DTMF команды. Исполняются в файле modem.cpp
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

// Датчики (не нужные модули закомментировать) 
 	
#define SENSOR_MOVE_ENABLE   	1// датчик движения
#define SENSOR_RADAR_ENABLE  	1// радар
#define SENSOR_FIRE_ENABLE 		1// датчик огня
#define SENSOR_GAS_ENABLE    	1// датчик дыма
#define SENSOR_DHT_ENABLE		1// датчик температуры и влажности DHT11 или DHT22
#define BEEP_ENABLE				1// пищалка

// Задание пинов для датчиков
// Дописать свои, если требуется. Со 2 по 4 пин изменять нельзя. Они зарезервированы.
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

/*
	НАСТРОЙКА ДАТЧИКОВ

	Инициализируем каждый датчик
	Если у вас другой датчик (не из списка), опишите его аналогично, и добавьте в SENSORS_INIT

    Номер параметра и значение
    1 - пин ардуино
    2 - тип датчика. Типы датчиков перечислены в "sensor.h"
    3 - уникальное имя датчика. Будет выводиться в отчётах.
    4 - уровен на цифровом пине датчика в спокойном режиме (LOW или  HIGHT)
    5 - время на подготовку датчика при старте в секундах,
    	когда к нему нельзя обращаться, чтобы не получить ложные данные.
    	Если не указать, то он будет равен 10 сек.
*/
#define DOOR_INIT 		Sensor(DOOR_PIN, DIGITAL_SENSOR, "DOOR", HIGH, 0)

#ifdef SENSOR_RADAR_ENABLE
	#define RADAR_INIT 	,Sensor(RADAR_PIN, DIGITAL_SENSOR, "RADAR", LOW)
#else 
	#define RADAR_INIT
#endif

#ifdef SENSOR_MOVE_ENABLE
	#define MOVE_INIT 	,Sensor(MOVE_PIN, DIGITAL_SENSOR, "MOVE", LOW)
#else
	#define MOVE_INIT
#endif

#ifdef SENSOR_FIRE_ENABLE
	#define FIRE_INIT 	,Sensor(FIRE_PIN, CHECK_DIGITAL_SENSOR, "FIRE", HIGH)
#else
	#define FIRE_INIT
#endif

// Инициализируем цифровые датчики с аналоговым выходoм.
// Для экономии пинов ардуино цифровой пин не используем
#ifdef SENSOR_GAS_ENABLE    
	#define GAS_INIT 	,Sensor(A0, ANALOG_SENSOR, "GAS", LOW, 120)
#else
	#define GAS_INIT
#endif

#ifdef SENSOR_DHT_ENABLE
	#define DHT_INIT 	,Sensor(DHT_PIN, DHT, "DHT")
#else
	#define DHT_INIT
#endif

// Сюда надо вписать свои датчики через пробел. Размер массива должен равняться количеству датчиков!!!
#define SENSORS_INIT Sensor sensors[6]={DOOR_INIT RADAR_INIT MOVE_INIT FIRE_INIT GAS_INIT DHT_INIT};

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

#define RESET_COUNT 				3 	// Сколько раз модем может не ответить до перезагрузки
#define ANALOG_SIGN_ALARM_VALUE 	200 // минимальное значение показаний аналоговых датчиков для тревоги
#define DHT_SIGN_ALARM_VALUE		35 	// минимальное значение показаний датчика температуры для тревоги
#define ALARM_MAX_TIME				60	// продолжительность тревоги sec, после чего счётчики срабатываний обнуляются

#endif // SETTINGS_H
