#include <iarduino_RTC.h>
//  iarduino_RTC time(RTC_DS1302, 8, 7, 6); // подключаем RTC модуль на базе чипа DS1302, указывая выводы Arduino подключённые к выводам модуля RST, CLK, DAT
//  iarduino_RTC time(RTC_DS1307);          // подключаем RTC модуль на базе чипа DS1307, используется аппаратная шина I2C
iarduino_RTC time(RTC_DS3231);          // подключаем RTC модуль на базе чипа DS3231, используется аппаратная шина I2C

#define OPEN_HOUR 9           // Час открытия
#define CLOSE_HOUR 21         // Час закрытия
#define PIN_RELAY_UP 5        // Определяем пин, используемый для подключения реле для ПОДЪЁМА
#define PIN_RELAY_DOWN 6      // Определяем пин, используемый для подключения реле для ОПУСКАНИЯ
#define PIN_GERCON_OPEN 3     // Определяем пин, используемый для подключения геркона остановки ПОДЪЁМА
#define PIN_GERCON_CLOSE 2    // Определяем пин, используемый для подключения геркона остановки ОПУСКАНИЯ
#define DOOR_IS_OPEN 1        // Значение возвращаемое в случае если дверь ОТкрыта
#define DOOR_IS_CLOSE 0       // Значение возвращаемое в случае если дверь ЗАкрыта
#define TESTING_PROGRAMM true    // Значение возвращаемое в случае если дверь ЗАкрыта

//--------------- Блок инициализации ------------
void setup() {
    digitalWrite(PIN_RELAY_UP, HIGH);     // Выключаем реле
    digitalWrite(PIN_RELAY_DOWN, HIGH);   // Выключаем реле

    delay(3000);
    Serial.begin(9600);
    time.begin();
//    time.settime(0,28,12,04,8,19,4);  // 0  сек, 07 мин, 22 час, 8, октября, 2019 год, четверг

    pinMode(PIN_RELAY_UP, OUTPUT);       // Объявляем пин реле как выход
    pinMode(PIN_RELAY_DOWN, OUTPUT);     // Объявляем пин реле как выход
    pinMode(PIN_GERCON_OPEN, INPUT);     // Объявляем пин геркона как вход
    pinMode(PIN_GERCON_CLOSE, INPUT);    // Объявляем пин геркона как вход
}
//--------------- Рабочий цикл программы ------------
void loop(){
  // Начинаем цикл.
  Serial.print("Now DateTime is: ");
  Serial.println(time.gettime("d-m-Y, H:i:s, D"));
  my_delay();

  if (IsPeriodToOpen()){
    Serial.print("It is period to open the door.  ");
    if (DoorStatus()!=DOOR_IS_OPEN){
      Serial.print("Trying to open the door.  ");
      MooveUp();
    } else {
      Serial.println("There is no reason to OPEN the Door.");
    }
  }
  
  if (IsPeriodToClose()){
    Serial.print("It is period to CLOSE the door.  ");
    if (DoorStatus()!=DOOR_IS_CLOSE){
      Serial.print("Trying to CLOSE the door.  ");
      MoveDown();
    } else {
      Serial.println("There is no reason to CLOSE the Door.");
    }
  }
}

//--------------- Функции состояний ------------
bool IsPeriodToOpen(){
  int nCurrTime;
  String sCurrTime;
  if (TESTING_PROGRAMM) {
    // В этом состоянии переключение режима ОТКРЫТЬ/ЗАКРЫТЬ
    // происходит каждые две минуты:
    // OPEN:  0,1,    4,5        и т.д.
    // CLOSE:     2,3,    6,7,   и т.д.
    sCurrTime = time.gettime("i");
    nCurrTime = sCurrTime.toInt();
    nCurrTime = nCurrTime - int(nCurrTime/4)*4;
    Serial.println(nCurrTime);
    return (nCurrTime==0 || nCurrTime==1);
  } else {
    // Если предыдущий код обошли, то отпределяем текущий час и 
    // входит ли он в установленный диапозон ограничений по часам.
    sCurrTime = time.gettime("H");
    nCurrTime = sCurrTime.toInt();
    return(nCurrTime>=OPEN_HOUR && nCurrTime<CLOSE_HOUR);
  }
}
bool IsPeriodToClose(){
  // Признак инвертированный от статуса "Открыть" .
  return(!IsPeriodToOpen());
}  
int DoorStatus(){
  if (digitalRead(PIN_GERCON_OPEN) && !digitalRead(PIN_GERCON_CLOSE)){
    Serial.println("STATUS: Door is open.");
    return(1);  // Открыта
  }
  if (digitalRead(PIN_GERCON_CLOSE) && !digitalRead(PIN_GERCON_OPEN)){
    Serial.println("STATUS: Door is close.");
    return(0);  // Закрыта
  }
  Serial.println("STATUS: Door is not open and not close!!!");
  return(-1); // Ни то, ни сё
}
//--------------- Функции работы с мотором ------------
void TurnOffEngine(){
  digitalWrite(PIN_RELAY_DOWN, HIGH);
  digitalWrite(PIN_RELAY_UP, HIGH);
  Serial.println("Engine stopped!");
}
void MooveUp(){
//  return
  Serial.println("Starting Engine....");
  digitalWrite(PIN_RELAY_DOWN, LOW);
  digitalWrite(PIN_RELAY_UP, HIGH);
  while (DoorStatus()!=DOOR_IS_OPEN){
    my_delay();
    Serial.println("...opening the Door....");
  }
  // Вышли из цикла по сигналу с геркона. Надо выключить мотор.
  TurnOffEngine();
}
void MoveDown(){
//  return
  Serial.println("Starting Engine....");
  digitalWrite(PIN_RELAY_UP, LOW);
  digitalWrite(PIN_RELAY_DOWN, HIGH);
  while (DoorStatus()!=DOOR_IS_CLOSE){
    my_delay();
    Serial.println("...closing the Door....");
  }
  // Вышли из цикла по сигналу с геркона. Надо выключить мотор.
  TurnOffEngine();
}
//----------------------------------------------------------------
void my_delay() {
  if (TESTING_PROGRAMM) {
    delay(5000);
  } else {
    delay(500);
  }
}

