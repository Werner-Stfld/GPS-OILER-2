#include <Arduino.h>
#include <ArduinoJson.h>
#include <unity.h>
#include "icons.h"
#include "userVar.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_icons_available() {

    TEST_ASSERT_NOT_NULL(iconOilcan());
    TEST_ASSERT_NOT_NULL(iconRaining());
    TEST_ASSERT_NOT_NULL(iconSatelite());
}

void test_float_usage() {
    float rainMulti = 2.51;
    long value = rainMulti;

    TEST_ASSERT_EQUAL(2, value); // fraction will be lost
    TEST_ASSERT_EQUAL(4, sizeof(rainMulti)); // float is 4 bytes
    value = rainMulti*100;
    TEST_ASSERT_EQUAL(251, value); // using percent keeps 2 fractional parts
    // rain Multiplier should better be a long value interpreted as percent. => time = measured time *rainMulti /100
}

IntVar TimeAPout = IntVar(5, PrefKeys::TimeAPout);                   // Zeit in Minuten bis sich der AP wieder abschaltet
StringVar ssid_ap = StringVar("GPS-OILER", PrefKeys::ssid_ap);      // Die SSID
void test_userVar() {
    int v = TimeAPout.get();
    TEST_ASSERT_EQUAL(5, v);
    TimeAPout.set(10, SetMode::flush);
    TimeAPout.set(8);
    TimeAPout.restore();
    v = TimeAPout.get();
    TEST_ASSERT_EQUAL(10, v);
}

void test_ssid_ap() {
    const char *v = ssid_ap.get();
    ssid_ap.set("GPS-OILER", SetMode::flush);
    ssid_ap.set("foo", SetMode::cache);

    ssid_ap.restore();
    v = ssid_ap.get();
    TEST_ASSERT_EQUAL(9, strlen(v));
    TEST_ASSERT_EQUAL(0, strcmp("GPS-OILER",v));
}

#pragma pack(push, 1)

union convert4 {
    int i;
    char c[4];
};
#pragma pack(pop)

void test_unions_encode() {
    convert4 conv;
    conv.i = 0x01 + (2 << 8) + (3 << 16) + (0x80 << 24);
    TEST_ASSERT_EQUAL_MESSAGE(&conv.i, conv.c, "fail: union returns same address for parts");
    TEST_ASSERT_EQUAL(0x01, conv.c[0]);
    TEST_ASSERT_EQUAL(0x02, conv.c[1]);
    TEST_ASSERT_EQUAL(0x03, conv.c[2]);
    TEST_ASSERT_EQUAL(0x80, conv.c[3]);
}

typedef char char20[20];

void test_unions_decode() {
    
    TEST_ASSERT_EQUAL(4, sizeof(int));
    TEST_ASSERT_EQUAL(4, sizeof(float));
    TEST_ASSERT_EQUAL( sizeof(int), sizeof(long));
    convert4 conv;
    conv.i = 0;
    conv.c[0] = 0x01;
    conv.c[1] = 0x02;
    conv.c[2] = 0x03;
    conv.c[3] = 0x80;
    TEST_ASSERT_EQUAL(0x01 + (2 << 8) + (3 << 16) + (0x80 << 24), conv.i);
}

void test_char20astype () {
    char20 foo = "0123456789012345678";
    TEST_ASSERT_EQUAL_MESSAGE((byte)'8', foo[18], "foo[18] != '8'"); 
    char20 bar;
    memcpy(bar, foo, sizeof(char20));
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(char20), 20, "sizeof != 20"); 
    bar[0]='0';
    //bar[18]='9';
    TEST_ASSERT_EQUAL_MESSAGE((byte)'0', bar[0], "bar[0] != '0'"); 
    TEST_ASSERT_EQUAL_MESSAGE((byte)'8', bar[18], "bar[18] != '9'"); 
}

void AddItems(JsonObject obj) {
    obj["foo"] = "bar";
}

void test_json() {
    String json;
    JsonDocument doc;

    JsonDocument docExpected;
    docExpected["a"]["foo"] = "bar";
    String jsonExpected;
    serializeJson(docExpected, jsonExpected);

    JsonObject aObj = doc["a"].to<JsonObject>();
    AddItems(aObj);
    serializeJson(doc, json);
    TEST_ASSERT_EQUAL_STRING(json.c_str(), jsonExpected.c_str()); 
}

void setup() {
    delay(300);
    Serial.begin(115200);

    delay(2000); // Wait for serial monitor
    UNITY_BEGIN();

    RUN_TEST(test_icons_available);
    RUN_TEST(test_userVar);
    RUN_TEST(test_float_usage);
    RUN_TEST(test_ssid_ap);
    RUN_TEST(test_char20astype);
    RUN_TEST(test_ssid_ap);
    RUN_TEST(test_unions_encode);
    RUN_TEST(test_unions_decode);
    RUN_TEST(test_char20astype);
    RUN_TEST(test_json);
    UNITY_END();
}

void loop () {
    return;
}