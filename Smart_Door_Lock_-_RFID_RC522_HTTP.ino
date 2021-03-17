//Created by @donnysdp//

#include <AntaresESP8266HTTP.h>                         //memasukan library antares http
#include <SPI.h>                                        //memasukan SPI bus library
#include <MFRC522.h>                                    //memasukan RFID reader library

#define ACCESSKEY "d787952530126057:f148cd2deef06033"   //user access key pada antares
#define WIFISSID "DLINK"                                //ssid wifi
#define PASSWORD "RTYUIOP999"                           //pass wifi 
#define projectName "smartdoorlock-RFID-HTTP"           //project name pada application antares
#define deviceName "frontdoor"                          //device name pada project name antares
#define RST_PIN 0                                       //pin RST pada RFID terhubung dengan pin D0
#define SS_PIN 2                                        //pin SS pada RFID terhubung dengan pin D2

String MasterTag = "4D7162";                            //simpan UID yang akan di accept dalam variabel MasterTag
String tagID = "";                                      //inisialisasi variabel untuk menyimpan UID yang didapat setelah proses tapping RFID Card/Keychain
MFRC522 mfrc522(SS_PIN, RST_PIN);                       //inisialisasi pembaca objek RFID

AntaresESP8266HTTP antares(ACCESSKEY);                  //konek ke antares http dengan accesskey

void setup()
{
  Serial.begin(115200);                                 //set baud rate ke 115200 bps
  antares.setDebug(true);                               //berfungsi utk menampilkan data-data apa saja yang dikerjakan oleh library antares selama library nya aktif (muncul di monitor)
  antares.wifiConnection(WIFISSID, PASSWORD);           //inisiasi start koneksi ke wifi

  SPI.begin();                                          //inisialisasi SPI
  mfrc522.PCD_Init();                                   //inisialisasi RFID Reader
}

void loop()
{
    Serial.println("Waiting RFID Card...");             //menampilkan tulisan di Serial Monitor
    delay(1500);                                        //delay 1500ms

  //////////////PROGRAM PENGGUNAAN RFID//////////////
  while (getID() == true)                                                 //di return oleh fungsi boolean getID dibawah karena mendapat return true
  { 
    if (tagID == MasterTag)                                               //jika tagID = MasterTag
    {
      Serial.print("Pintu dibuka menggunakan RFID dengan UID : ");        //menampilkan tulisan di Serial Monitor
      Serial.println(tagID);                                              //menampilkan isi dari variabel tagID
      delay(2000);
      Serial.println();                                                   //membuat fungsi enter pada Serial Monitor
      antares.add("PASS : Pintu dibuka menggunakan RFID dengan UID : ", tagID);  //menambahkan pada "wadah" antares untuk menampilkan tulisan di web antares
      antares.send(projectName, deviceName);                              //mengirimkan "wadah" ke antares untuk ditampilkan/diproses
    }
    else                                                                  //jika tagID != MasterTag
    {
      Serial.print("UID tidak terdaftar : ");                             //menampilkan tulisan di Serial Monitor
      Serial.println(tagID);                                              //menampilkan isi dari variabel tagID
      delay(1000);                                                        //delay 1s
      Serial.println();                                                   //membuat fungsi enter pada Serial Monitor
      antares.add("DENIED : Pintu dibuka menggunakan UID yang salah : ", tagID);   //menambahkan pada "wadah" antares untuk menampilkan tulisan di web antares
      antares.send(projectName, deviceName);                              //mengirimkan "wadah" ke antares untuk ditampilkan/diproses
    }
  }
}


///////Proses RFID Sensor Membaca PICC Card//////////
boolean getID()                                         //boolean getID mendapatkan sinyal/informasi akan RFID Card/Tag yang terdeteksi
{
  if(!mfrc522.PICC_IsNewCardPresent())                  //Jika ada PICC terdeteksi oleh RFID Reader, maka lanjutkan
  {  
    return false;
  }
  if(!mfrc522.PICC_ReadCardSerial())                    //Jika PICC Serial nya kebaca, maka lanjutkan
  {
    return false;
  }
  tagID = "";                                           //variabel untuk menyimpan UID 
  for(int i = 0; i < 4; i++)                            //PICC yang kita gunakan memiliki 4 byte UID, maka dari itu perulangannya seperti itu
  { 
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));  //menggabungkan beberapa string kedalam variable tagID
  }
  tagID.toUpperCase();                                  //membuat karakter yang ada pada variable tagID menjadi huruf kapital semua
  mfrc522.PICC_HaltA();                                 //membuat RFID Scanner berhenti untuk scanning RFID Card
  return true;                                          //kembali ke void loop()
}
