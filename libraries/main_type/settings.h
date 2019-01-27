#ifndef SETTINGS_H
#define SETTINGS_H

/*
 * В этом файле настройки осуществляются дефайнами
 * Для выбора настроки в дефайне указывают 1.
 * Для запрета указывают 0.
 */

//------------------------------------------------------------------------------
// Вывод отладочных сообщений в Serial
// В рабочем режиме вывод должен быть запрещён
#define DEBUG_MODE 0
//------------------------------------------------------------------------------

#include "main_type.h"

/*
 *	Далее идут индивидульные настройки как сигнализации,
 *	так и беспроводного датчика.
 *	Для прошивки сигнализации в #if пишем 1.
 *	Для прошивки беспроводного датчика в #if пишем 0.
 *	Если #if 1, то компилируется код с 26 до 138 строки, относящийся к скетчу Signalka.ino
 *	Если #if 0, то компилируется код 138 строки и ниже, относящийся к скетчу IRsensor.ino
 */
#if 1 // Настройки сигнализации Signalka.ino

//////////////////////////////////////////////////////////
// Подключаемые библиотеки.
// Для отключения библиотеки и экономии памяти 1 меняем на 0 и наоборот
//////////////////////////////////////////////////////////
#	define WTD_ENABLE	0 // Используем watchdog
#	define IR_ENABLE	1 // библиотека для ик приёмника
#	define BEEP_ENABLE	0 // пищалка, не работает совместно с IR_ENABLE
#	define DHT_ENABLE	1 // библиотека датчиков DHT11, DHT21 и DHT22
#	define TERM_ENABLE	1 // библиотека для термистора
//////////////////////////////////////////////////////////

//****************************************************
// Выбор способа получения отчётов (или GPRS, или SMS)
//****************************************************
#	define OTCHET_INIT	SET_FLAG_ONE(GPRS_ENABLE);SET_FLAG_ONE(SMS_ENABLE);
//****************************************************

// DTMF команды. Исполняются в файле modem.cpp
enum {
	GUARD_ON = 1,		// 1# - постановка на охрану
	GUARD_OFF,			// 2# - снятие с охраны	
	GPRS_ON_OFF,		// 3# - включить/отключить GPRS
	SMS_ON_OFF,			// 4# - включить/отключить SMS
	TEL_ON_OFF,			// 5# - включить/отключить звонок при тревоге
	GET_INFO,			// 6# - сбор и отправка всех данных датчиков
	EMAIL_ADMIN_PHONE,	// 7# - отправляем на почту номер админа
	EMAIL_PHONE_BOOK,	// 8# - отправка на почту телефонной книги
	ADMIN_NUMBER_DEL,	// 9# - админ больше не админ
	SM_CLEAR,			// 10# - удалить все номера с симкарты
	MODEM_RESET,		// 11# - перезагрузка модема
	BAT_CHARGE			// 12# - показывает заряд батареи в виде строки
						//  +CBC: 0,100,4200
						// где 100 - процент заряда
						// 4200 - напряжение на батарее в мВ.
	};

/*
	НАСТРОЙКА ДАТЧИКОВ
*/

// Задание пинов для датчиков
// Можно дописать свои и удалить лишние. Первые 3 пина изменять нельзя. Они зарезервированы.
enum pins {
	RING_PIN = 2,	//2 отслеживает вызовы с модема
	POWER_PIN,		//3 отслеживает наличие питания
	DOOR_PIN,		//4 датчик двери (геркон). Один конец на GND, второй на цифровой пин arduino.
	MOVE_PIN,   	//5 все датчики движения вешаем на один пин
	RADAR_PIN,   	//6 RCWL-0516
	FIRE_PIN,		//7 датчик огня
	DHT_PIN,   		//8 датчик температуры и влажности DHT11, DHT21 или DHT22
	BEEP_PIN,		//9 пищалка
	RECV_PIN = 11,	//11 ИК приёмник
	BOOT_PIN		//12 перезагрузка модема
	};
/*
	Инициализируем каждый датчик
	Если у вас другой датчик (не из списка), опишите его аналогично, и добавьте в SENSORS_INIT

   	Номер параметра и значение (см. sensor.cpp)
   	1 - пин ардуино
   	2 - тип датчика. Типы датчиков перечислены в "sensor.h"
   	3 - уникальное имя датчика. Будет выводиться в отчётах.
   	4 - уровен на цифровом пине датчика в спокойном режиме (LOW или  HIGHT)
   	5 - время на подготовку датчика при старте в секундах,
   		когда к нему нельзя обращаться, чтобы не получить ложные данные.
   		Если не указать, то он будет равен 10 сек.
   	6 - порог срабатывания аналогового датчика, по умолчанию равен 200

   	Для датчиков типа IR_SENSOR задаётся всего 3 параметра

   	Номер параметра и значение (см. sensor.cpp)
   	1 - тип датчика IR_SENSOR
   	2 - уникальное имя датчика. Будет выводиться в отчётах.
   	3 - 32 разрядное кодовое слово, передаваемое беспроводным датчиком при срабатывании
*/

// Сюда надо вписать свои датчики. Размер массива должен равняться количеству датчиков!
#	define SENSORS_INIT Sensor sensors[8]={ \
		Sensor(DOOR_PIN,	DIGITAL_SENSOR,		"DOOR", 	HIGH,	0), 	\
		Sensor(RADAR_PIN,	DIGITAL_SENSOR,		"RADAR",	LOW), 			\
		Sensor(MOVE_PIN,	DIGITAL_SENSOR,		"MOVE", 	LOW), 			\
		Sensor(FIRE_PIN,	CHECK_DIGITAL_SENSOR,	"FIRE", 	HIGH),			\
		Sensor(			IR_SENSOR,		"IR_0",		0x41038C7),		\
		Sensor(A0,		ANALOG_SENSOR,		"GAS",		LOW,	120),	\
		Sensor(A1,		TERMISTOR,		"TERM",		LOW,	10, 45),\
		Sensor(DHT_PIN,		DHT11,			"DHT",		LOW,	10,	45)};

	//*****************************************************************
	//////////////////////////////////////////////////////////
	/// Изменить параметры под себя
	//////////////////////////////////////////////////////////
	// Отправка почты
#	define SMTP_SERVER			F("\"smtp-devices.yandex.com\",25") // почтовый сервер яндекс и порт
#	define SMTP_USER_NAME_AND_PASSWORD	F("\"login\",\"password\"") // Лоргин и пароль от почты
#	define SENDER_ADDRESS_AND_NAME		F("\"login@yandex.com\",\"SIM800L\"")
#	define RCPT_ADDRESS_AND_NAME		F("\"login@mail.ru\",\"Ivan\"") // Адрес и имя получателя
//#	define RCPT_CC_ADDRESS_AND_NAME		F("\"login@yandex.com\",\"Ivan\"") // Адрес и имя получателя (копия)
//#	define RCPT_BB_ADDRESS_AND_NAME		F("\"login2@yandex.com\",\"Ivan\"") // Адрес и имя получателя (вторая копия)

	//////////////////////////////////////////////////////////
	/// Энергосбережение
	//////////////////////////////////////////////////////////
#	define SLEEP_MODE_ENABLE	1 // Разрешаем спящий режим для экономии батареи
	//////////////////////////////////////////////////////////
	/// Прочие настройки
	//////////////////////////////////////////////////////////
#	define SERIAL_RATE		115200 // скорость последовательного порта Serial
#	define RESET_COUNT		3 	// Сколько раз модем может не ответить до перезагрузки
#	define ALARM_MAX_TIME	60	// продолжительность тревоги в секундах, после чего счётчики срабатываний обнуляются

#else	// Настройки беспроводного ик датчика IRsensor.ino

//////////////////////////////////////////////////////////
// Подключаемые библиотеки.
//////////////////////////////////////////////////////////
#	define WTD_ENABLE	0 // Используем watchdog
#	define DHT_ENABLE	0 // библиотека датчиков DHT11, DHT21 и DHT22
#	define TERM_ENABLE	0 // библиотека для термистора
//////////////////////////////////////////////////////////

#	define IR_CODE 			0x41038C7	// данный код передатчик будет отправлять приёмнику
#	define IR_CODE_BIT_SIZE 32 			// размер кода в битах

	/*
		НАСТРОЙКА ДАТЧИКОВ
	*/

	// Задание пинов для датчиков надо начинать с 4 пина.
	// Можно дописать свои и удалить лишние.
	enum pins {
		DOOR_PIN = 4,	//4 датчик двери (геркон). Один конец на GND, второй на цифровой пин arduino.
		MOVE_PIN	   	//5 все датчики движения вешаем на один пин
	};

	// Сюда надо вписать свои датчики. Размер массива должен равняться количеству датчиков!
#	define SENSORS_INIT Sensor sensors[2]={ \
		Sensor(DOOR_PIN, DIGITAL_SENSOR, "DOOR", HIGH, 0),\
		Sensor(MOVE_PIN, DIGITAL_SENSOR, "MOVE", LOW)};
	//////////////////////////////////////////////////////////
	/// Прочие настройки
	//////////////////////////////////////////////////////////
#	define ALARM_MAX_TIME	10	// продолжительность отправки сообщения при срабатывании датчика, в секундах

#endif // SIGNALKA

#endif // SETTINGS_H
