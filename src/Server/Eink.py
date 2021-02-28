import threading
import time
import json
import re
import queue
import paho.mqtt.client as mqtt
import requests
import jsonpath
import socketio

import config
import dic

from fastapi import FastAPI
import uvicorn

app = FastAPI()

data_api = "0"

client = mqtt.Client()

sio = socketio.Client()

lock_a = 1
lock_b = 0
q = queue.Queue(maxsize=1)
q_notice = queue.Queue(maxsize=1)


@app.get("/hello")
def read_root():
    return {"hello": "world"}


@app.get("/items/{item_id}")
def read_item(item_id: int, q: str = None):
    return {"item_id": item_id, "q": q}


@app.get("/temperature")
def get_temperature():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_temperature', payload='1', qos=0)
    print("温度api")
    i = 0
    global data_api
    while i < 100:
        if data_api != '0':
            data = data_api
            data_api = '0'
            return data
        time.sleep(0.1)
        i = i + 1
    return '未获取到信息'


@app.get("/temperature_r")
def get_temperature_r():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_temperature_r', payload='1', qos=0)
    print("温度api")
    i = 0
    global data_api
    while i < 100:
        if data_api != '0':
            data = data_api
            data_api = '0'
            return data
        time.sleep(0.1)
        i = i + 1
    return '未获取到信息'


@app.get("/pressure")
def get_pressure():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_pressure', payload='1', qos=0)
    print("气压api")
    i = 0
    global data_api
    while i < 100:
        if data_api != '0':
            data = data_api
            data_api = '0'
            return data
        time.sleep(0.1)
        i = i + 1
    return '未获取到信息'


@app.get("/pressure_r")
def get_pressure_r():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_pressure_r', payload='1', qos=0)
    print("气压api")
    i = 0
    global data_api
    while i < 100:
        if data_api != '0':
            data = data_api
            data_api = '0'
            return data
        time.sleep(0.1)
        i = i + 1
    return '未获取到信息'


@app.get("/humidity")
def get_humidity():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_humidity', payload='1', qos=0)
    print("湿度api")
    i = 0
    global data_api
    while i < 100:
        if data_api != '0':
            data = data_api
            data_api = '0'
            return data
        time.sleep(0.1)
        i = i + 1
    return '未获取到信息'


@app.get("/humidity_r")
def get_humidity_r():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_humidity_r', payload='1', qos=0)
    print("湿度api")
    i = 0
    global data_api
    while i < 100:
        if data_api != '0':
            data = data_api
            data_api = '0'
            return data
        time.sleep(0.1)
        i = i + 1
    return '未获取到信息'


@app.get("/iaq")
def get_iqa():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_iaq', payload='1', qos=0)
    print("iaq api")
    i = 0
    global data_api
    while i < 100:
        if data_api != '0':
            data = data_api
            data_api = '0'
            return data
        time.sleep(0.1)
        i = i + 1
    return '未获取到信息'


@app.get("/led_on")
def led_on_api():
    client.publish('to_led', payload='1', qos=0)


@app.get("/led_off")
def led_off_api():
    client.publish('to_led', payload='0', qos=0)


@app.get("/printer_on")
def printer_on_api():
    client.publish('to_printer', payload='1', qos=0)


@app.get("/printer_off")
def printer_off_api():
    client.publish('to_printer', payload='0', qos=0)


@app.get("/weather/city1/{city}")
def set_city1(city: str):
    config.b_city_1 = city
    weather_call()
    return "设置城市1为" + city


@app.get("/weather/city1_r/{city}")
def set_city1_r(city: str):
    config.r_city_1 = city
    weather_call_r()
    return "设置城市1为" + city


@app.get("/weather/city2/{city}")
def set_city2(city: str):
    config.b_city_2 = city
    weather_call()
    return "设置城市2为" + city


@app.get("/weather/city2_r/{city}")
def set_city2_r(city: str):
    config.r_city_2 = city
    weather_call_r()
    return "设置城市2为" + city


@app.get("/notice/b/{notice}")
def notice_api(notice: str):
    if q_notice.full():
        q_notice.queue.clear()
    q_notice.put('api')
    client.publish('notice', payload=notice, qos=0)
    print("消息api")
    i = 0
    global data_api
    while i < 100:
        if data_api != '0':
            data = data_api
            data_api = '0'
            return data
        time.sleep(0.1)
        i = i + 1
    return '未发送成功'


@app.get("/notice/r/{notice}")
def notice_r_api(notice: str):
    if q_notice.full():
        q_notice.queue.clear()
    q_notice.put('api')
    client.publish('notice_r', payload=notice, qos=0)
    print("消息api")
    i = 0
    global data_api
    while i < 100:
        if data_api != '0':
            data = data_api
            data_api = '0'
            return data
        time.sleep(0.1)
        i = i + 1
    return '未发送成功'


def weather_call():
    url = "https://free-api.heweather.net/s6/weather/forecast"
    payload = {
        'location': config.b_city_1,
        'key': config.QWEATHER_KEY
    }
    r = requests.get(url, params=payload)
    data = json.loads(r.text)
    code = jsonpath.jsonpath(data, '$..cond_code_d')
    wea = jsonpath.jsonpath(data, '$..cond_txt_d')
    tmp_min = jsonpath.jsonpath(data, '$..tmp_min')
    tmp_max = jsonpath.jsonpath(data, '$..tmp_max')
    dic.weather_dic['code_1'] = dic.code_dic[code[0]]
    dic.weather_dic['wea_1'] = wea[0]
    dic.weather_dic['tmp_1_min'] = tmp_min[0]
    dic.weather_dic['tmp_1_max'] = tmp_max[0]
    payload = {
        'location': config.b_city_2,
        'key': config.QWEATHER_KEY
    }
    r = requests.get(url, params=payload)
    data = json.loads(r.text)
    code = jsonpath.jsonpath(data, '$..cond_code_d')
    wea = jsonpath.jsonpath(data, '$..cond_txt_d')
    tmp_min = jsonpath.jsonpath(data, '$..tmp_min')
    tmp_max = jsonpath.jsonpath(data, '$..tmp_max')
    dic.weather_dic['code_2'] = dic.code_dic[code[0]]
    dic.weather_dic['wea_2'] = wea[0]
    dic.weather_dic['tmp_2_min'] = tmp_min[0]
    dic.weather_dic['tmp_2_max'] = tmp_max[0]
    weather_json = json.dumps(dic.weather_dic, ensure_ascii=False)
    print(weather_json)
    client.publish('weather', payload=weather_json, qos=0)


def weather_call_r():
    url = "https://free-api.heweather.net/s6/weather/forecast"
    payload = {
        'location': config.r_city_1,
        'key': config.QWEATHER_KEY
    }
    r = requests.get(url, params=payload)
    data = json.loads(r.text)
    code = jsonpath.jsonpath(data, '$..cond_code_d')
    wea = jsonpath.jsonpath(data, '$..cond_txt_d')
    tmp_min = jsonpath.jsonpath(data, '$..tmp_min')
    tmp_max = jsonpath.jsonpath(data, '$..tmp_max')
    dic.weather_dic_r['code_1'] = dic.code_dic[code[0]]
    dic.weather_dic_r['wea_1'] = wea[0]
    dic.weather_dic_r['tmp_1_min'] = tmp_min[0]
    dic.weather_dic_r['tmp_1_max'] = tmp_max[0]
    payload = {
        'location': config.r_city_2,
        'key': config.QWEATHER_KEY
    }
    r = requests.get(url, params=payload)
    data = json.loads(r.text)
    code = jsonpath.jsonpath(data, '$..cond_code_d')
    wea = jsonpath.jsonpath(data, '$..cond_txt_d')
    tmp_min = jsonpath.jsonpath(data, '$..tmp_min')
    tmp_max = jsonpath.jsonpath(data, '$..tmp_max')
    dic.weather_dic_r['code_2'] = dic.code_dic[code[0]]
    dic.weather_dic_r['wea_2'] = wea[0]
    dic.weather_dic_r['tmp_2_min'] = tmp_min[0]
    dic.weather_dic_r['tmp_2_max'] = tmp_max[0]
    weather_json = json.dumps(dic.weather_dic_r, ensure_ascii=False)
    print(weather_json)
    client.publish('weather_r', payload=weather_json, qos=0)


# def temperature_lock():
#     while True:
#         data = q.get(block=True,timeout=None)
#         send_to_user('854214642',"当前温度:"+data)

# mqtt 连接成功回调函数
def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))


# mqtt 接收信息成功回调函数
def on_message(client, userdata, msg):
    msg.payload = msg.payload.decode('utf-8')
    str_test = msg.topic + " " + str(msg.payload)
    print(str_test)
    global data_api
    if msg.topic == 'door':
        send_to_user('854214642', "门的当前状态为:" + str(msg.payload))
    elif msg.topic == 'from_temperature':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api = "当前温度:" + str(float(str(msg.payload)) / 10) + '℃'
            else:
                send_to_user(data, "当前温度:" + str(float(str(msg.payload)) / 10) + '℃')
        print("当前温度:" + str(msg.payload) + '℃')
    elif msg.topic == 'from_humidity':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api = "当前湿度:" + str(msg.payload) + '%'
            else:
                send_to_user(data, "当前湿度:" + str(msg.payload) + '%')
        print("当前湿度:" + str(msg.payload) + '%')
    elif msg.topic == 'from_iaq':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api = "当前空气指数:" + str(msg.payload)
            else:
                send_to_user(data, "当前空气指数:" + str(msg.payload))
        print("当前空气指数:" + str(msg.payload))
    elif msg.topic == 'from_pressure':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api = "当前气压:" + str(msg.payload) + 'mPa'
            else:
                send_to_user(data, "当前气压:" + str(msg.payload) + 'mPa')
        print("当前气压:" + str(msg.payload) + 'mPa')
    elif msg.topic == 'notice_back':
        if not q_notice.empty():
            data = q_notice.get()
            if data == 'api':
                data_api = "已经发送成功了吼吼"
            else:
                send_to_user(data, "已经发送成功了吼吼")
    elif msg.topic == 'from_temperature_r':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api = "当前温度:" + str(float(str(msg.payload)) / 10) + '℃'
            else:
                send_to_user(data, "当前温度:" + str(float(str(msg.payload)) / 10) + '℃')
        print("当前温度:" + str(msg.payload) + '℃')
    elif msg.topic == 'from_humidity_r':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api = "当前湿度:" + str(msg.payload) + '%'
            else:
                send_to_user(data, "当前湿度:" + str(msg.payload) + '%')
        print("当前湿度:" + str(msg.payload) + '%')
    elif msg.topic == 'from_pressure_r':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api = "当前气压:" + str(msg.payload) + 'mPa'
            else:
                send_to_user(data, "当前气压:" + str(msg.payload) + 'mPa')
        print("当前气压:" + str(msg.payload) + 'mPa')
    elif msg.topic == 'notice_back_r':
        if not q_notice.empty():
            data = q_notice.get()
            if data == 'api':
                data_api = "已经发送成功了吼吼"
            else:
                send_to_user(data, "已经发送成功了吼吼")
    elif msg.topic == 'weather_call':
        weather_call()
    elif msg.topic == 'weather_call_r':
        weather_call_r()


# mqtt 初始化函数
def mqtt_init():
    time.sleep(0.2)
    client.on_connect = on_connect
    client.on_message = on_message
    # client.on_disconnect = on_disconnect
    client.connect(config.MQTT_Address, config.MQTT_Port, 600)  # 600为keepalive的时间间隔
    client.subscribe('from_temperature', qos=0)
    client.subscribe('weather_call', qos=0)
    client.subscribe('notice_back', qos=0)
    client.subscribe('from_humidity', qos=0)
    client.subscribe('from_iaq', qos=0)
    client.subscribe('from_pressure', qos=0)
    client.subscribe('from_temperature_r', qos=0)
    client.subscribe('weather_call_r', qos=0)
    client.subscribe('notice_back_r', qos=0)
    client.subscribe('from_humidity_r', qos=0)
    # client.subscribe('from_iaq',qos=0)
    client.subscribe('from_pressure_r', qos=0)
    client.loop_forever()  # 保持连接


@sio.event
def connect():
    print('connected to server')


@sio.on('OnFriendMsgs')
def OnFriendMsgs(message):
    """ 监听好友消息 """
    print('message:' + message['CurrentPacket']['Data']['Content'])
    print('QQ:' + str(message['CurrentPacket']['Data']['FromUin']))
    if str(message['CurrentPacket']['Data']['FromUin']) in config.admin_user:
        # print("你是超级管理员")
        if re.search('开灯', message['CurrentPacket']['Data']['Content'], re.I):
            client.publish('to_led', payload='1', qos=0)
            send_to_user(str(message['CurrentPacket']['Data']['FromUin']), "好的，灯打开了！")
            print("开灯")
        elif re.search('关灯', message['CurrentPacket']['Data']['Content'], re.I):
            client.publish('to_led', payload='0', qos=0)
            send_to_user(str(message['CurrentPacket']['Data']['FromUin']), "好的，灯关闭了！")
            print("关灯")
        elif re.search('开打印机', message['CurrentPacket']['Data']['Content'], re.I):
            client.publish('to_printer', payload='1', qos=0)
            send_to_user(str(message['CurrentPacket']['Data']['FromUin']), "好的，打印机打开了！")
            print("开打印机")
        elif re.search('关打印机', message['CurrentPacket']['Data']['Content'], re.I):
            client.publish('to_printer', payload='0', qos=0)
            send_to_user(str(message['CurrentPacket']['Data']['FromUin']), "好的，打印机关闭了！")
            print("关打印机")
        elif message['CurrentPacket']['Data']['Content'][0] == config.Ink2_2_NoticeOrder:
            if q_notice.full():
                q_notice.queue.clear()
            q_notice.put(str(message['CurrentPacket']['Data']['FromUin']))
            client.publish('notice', payload=message['CurrentPacket']['Data']['Content'][1:], qos=0)
        elif message['CurrentPacket']['Data']['Content'][0] == config.Ink2_3_NoticeOrder:
            if q_notice.full():
                q_notice.queue.clear()
            q_notice.put(str(message['CurrentPacket']['Data']['FromUin']))
            client.publish('notice_r', payload=message['CurrentPacket']['Data']['Content'][1:], qos=0)
        elif re.search(config.Ink2_3_SensorOrder, message['CurrentPacket']['Data']['Content'], re.I):
            if re.search('温度', message['CurrentPacket']['Data']['Content'], re.I):
                if q.full():
                    q.queue.clear()
                q.put(str(message['CurrentPacket']['Data']['FromUin']))
                client.publish('to_temperature_r', payload='1', qos=0)
                print("温度")
            elif re.search('湿度', message['CurrentPacket']['Data']['Content'], re.I):
                if q.full():
                    q.queue.clear()
                q.put(str(message['CurrentPacket']['Data']['FromUin']))
                client.publish('to_humidity_r', payload='1', qos=0)
                print("湿度")
            elif re.search('气压', message['CurrentPacket']['Data']['Content'], re.I):
                if q.full():
                    q.queue.clear()
                q.put(str(message['CurrentPacket']['Data']['FromUin']))
                client.publish('to_pressure_r', payload='1', qos=0)
                print("气压")
        elif re.search(config.Ink2_2_SensorOrder, message['CurrentPacket']['Data']['Content'], re.I):
            if re.search('温度', message['CurrentPacket']['Data']['Content'], re.I):
                if q.full():
                    q.queue.clear()
                q.put(str(message['CurrentPacket']['Data']['FromUin']))
                client.publish('to_temperature', payload='1', qos=0)
                print("温度")
            elif re.search('湿度', message['CurrentPacket']['Data']['Content'], re.I):
                if q.full():
                    q.queue.clear()
                q.put(str(message['CurrentPacket']['Data']['FromUin']))
                client.publish('to_humidity', payload='1', qos=0)
                print("湿度")
            elif re.search('气压', message['CurrentPacket']['Data']['Content'], re.I):
                if q.full():
                    q.queue.clear()
                q.put(str(message['CurrentPacket']['Data']['FromUin']))
                client.publish('to_pressure', payload='1', qos=0)
                print("气压")
            elif re.search('空气', message['CurrentPacket']['Data']['Content'], re.I):
                if q.full():
                    q.queue.clear()
                q.put(str(message['CurrentPacket']['Data']['FromUin']))
                client.publish('to_iaq', payload='1', qos=0)
                print("空气")
    return


@sio.on('OnEvents')
def OnEvents(message):
    """ 监听相关事件"""
    print(message)


@sio.event
def disconnect():
    print('disconnected from server')


# ws发送给用户
def send_to_user(user, message):
    msg_data = {
        "toUser": int(user),  # 欲发给的对象 群或QQ好友或私聊对象
        "sendToType": 1,  # 发送消息对象的类型 1好友 2群3私聊
        "sendMsgType": 'TextMsg',  # 欲发送消息的类型 TextMsg、PicMsg、VoiceMsg
        "content": message,  # 发送的文本内容
        "groupid": 0,  # 发送私聊消息是 在此传入群ID 其他情况为0
        "atUser": 0,  # At用户 传入用户的QQ号 其他情况为0
        "picUrl": '',  # 发送图片的网络地址
        "picBase64Buf": '',  # 发本地送图片的buf 转 bas64 编码
        "voiceUrl": '',  # 发送语音的网络地址
        "voiceBase64Buf": '',  # 发本地送语音的buf 转 bas64 编码
        "fileMd5": ""
    }
    requests.post(url=config.OPQ_SocketIO + f'/v1/LuaApiCaller?qq=' + config.OPQ_QQ + f'&funcname=SendMsg&timeout=10',
                  data=json.dumps(msg_data))


def ws_init():
    sio.connect(config.OPQ_SocketIO, transports=['websocket'])
    sio.wait()


def main():
    t1 = threading.Thread(target=ws_init)
    t2 = threading.Thread(target=mqtt_init)
    t1.start()
    t2.start()


if __name__ == '__main__':
    main()
    uvicorn.run(app=app, host="0.0.0.0", port=config.API_Port, log_level="info")
