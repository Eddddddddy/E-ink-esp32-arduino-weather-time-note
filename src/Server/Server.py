#导入包
import websocket
import threading
import time
import json
import re
import queue
import paho.mqtt.client as mqtt
import requests
import jsonpath

from fastapi import FastAPI
import datetime
#import psutil
import uvicorn

#在这里输入你要添加的超级用户
admin_user=('xxxxxxxxxx') 

b_city_1='唐山'
b_city_2='石家庄'
r_city_1='石家庄'
r_city_2='唐山'

app = FastAPI()

data_api="0"







#上线问候语
str_1="{\"action\": \"send_private_msg\",\"params\": {\"user_id\": "你自己的QQ号",\"message\": \"我上线啦！！！\"}}"

weather_dic={
    'code_1':'null',
    'code_2':'null',
    'wea_1':'null',
    'wea_2':'null',
    'tmp_1_min':'null',
    'tmp_1_max':'null',
    'tmp_2_min':'null',
    'tmp_2_max':'null'
}

weather_dic_r={
    'code_1':'null',
    'code_2':'null',
    'wea_1':'null',
    'wea_2':'null',
    'tmp_1_min':'null',
    'tmp_1_max':'null',
    'tmp_2_min':'null',
    'tmp_2_max':'null'
}

code_dic = {
    '100'   :'0',
    '100n'  :'1',
    '101'   :'2',
    '102'   :'3',
    '103'   :'4',
    '103n'  :'5',
    '104'   :'6',
    '104n'  :'7',
    '200'   :'8',
    '201'   :'8',
    '202'   :'8',
    '203'   :'8',
    '204'   :'8',
    '205'   :'9',
    '206'   :'9',
    '207'   :'9',
    '208'   :'10',
    '209'   :'10',
    '210'   :'10',
    '211'   :'10',
    '212'   :'10',
    '213'   :'10',
    '300'   :'11',
    '300n'  :'12',
    '301'   :'13',
    '301n'  :'14',
    '302'   :'15',
    '303'   :'16',
    '304'   :'17',
    '305'   :'18',
    '306'   :'19',
    '314'   :'19',
    '307'   :'20',
    '315'   :'20',
    '309'   :'21',
    '310'   :'22',
    '316'   :'22',
    '311'   :'23',
    '317'   :'23',
    '312'   :'24',
    '318'   :'24',
    '313'   :'25',
    '399'   :'26',
    '400'   :'27',
    '401'   :'28',
    '408'   :'28',
    '402'   :'29',
    '409'   :'29',
    '403'   :'30',
    '410'   :'30',
    '404'   :'31',
    '405'   :'32',
    '406'   :'33',
    '406n'  :'34',
    '407'   :'35',
    '407n'  :'36',
    '499'   :'37',
    '500'   :'38',
    '501'   :'39',
    '502'   :'40',
    '503'   :'41',
    '504'   :'42',
    '507'   :'43',
    '508'   :'44',
    '509'   :'45',
    '510'   :'45',
    '514'   :'45',
    '515'   :'45',
    '511'   :'46',
    '512'   :'47',
    '513'   :'48',
    '900'   :'49',
    '901'   :'49',
    '999'   :'50'
}

websocket.enableTrace(False)

my_ws = websocket.WebSocketApp("ws://127.0.0.1:6700")
client = mqtt.Client()

lock_a=1
lock_b=0
q = queue.Queue(maxsize=1)
q_notice = queue.Queue(maxsize=1)

@app.get("/hello")
def read_root():
    return {"hello":"world"}

@app.get("/items/{item_id}")
def read_item(item_id: int, q:str = None):
    return {"item_id":item_id,"q":q}

@app.get("/temperature")
def get_temperature():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_temperature',payload='1',qos=0)
    print("温度api")
    i=0
    global data_api
    while i<100:
        if data_api != '0':
            data = data_api
            data_api='0'
            return data
        time.sleep(0.1)
        i=i+1
    return '未获取到信息'

@app.get("/temperature_r")
def get_temperature_r():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_temperature_r',payload='1',qos=0)
    print("温度api")
    i=0
    global data_api
    while i<100:
        if data_api != '0':
            data = data_api
            data_api='0'
            return data
        time.sleep(0.1)
        i=i+1
    return '未获取到信息'

@app.get("/pressure")
def get_pressure():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_pressure',payload='1',qos=0)
    print("气压api")
    i=0
    global data_api
    while i<100:
        if data_api != '0':
            data = data_api
            data_api='0'
            return data
        time.sleep(0.1)
        i=i+1
    return '未获取到信息'

@app.get("/pressure_r")
def get_pressure_r():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_pressure_r',payload='1',qos=0)
    print("气压api")
    i=0
    global data_api
    while i<100:
        if data_api != '0':
            data = data_api
            data_api='0'
            return data
        time.sleep(0.1)
        i=i+1
    return '未获取到信息'

@app.get("/humidity")
def get_humidity():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_humidity',payload='1',qos=0)
    print("湿度api")
    i=0
    global data_api
    while i<100:
        if data_api != '0':
            data = data_api
            data_api='0'
            return data
        time.sleep(0.1)
        i=i+1
    return '未获取到信息'

@app.get("/humidity_r")
def get_humidity_r():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_humidity_r',payload='1',qos=0)
    print("湿度api")
    i=0
    global data_api
    while i<100:
        if data_api != '0':
            data = data_api
            data_api='0'
            return data
        time.sleep(0.1)
        i=i+1
    return '未获取到信息'

@app.get("/iaq")
def get_iqa():
    if q.full():
        q.queue.clear()
    q.put('api')
    client.publish('to_iaq',payload='1',qos=0)
    print("iaq api")
    i=0
    global data_api
    while i<100:
        if data_api != '0':
            data = data_api
            data_api='0'
            return data
        time.sleep(0.1)
        i=i+1
    return '未获取到信息'

@app.get("/led_on")
def led_on_api():
    client.publish('to_led',payload='1',qos=0)

@app.get("/led_off")
def led_off_api():
    client.publish('to_led',payload='0',qos=0)

@app.get("/printer_on")
def printer_on_api():
    client.publish('to_printer',payload='1',qos=0)

@app.get("/printer_off")
def printer_off_api():
    client.publish('to_printer',payload='0',qos=0)
    
@app.get("/weather/city1/{city}")
def set_city1(city: str):
    global b_city_1
    b_city_1=city
    weather_call()
    return "设置城市1为"+city

@app.get("/weather/city1_r/{city}")
def set_city1_r(city: str):
    global r_city_1
    r_city_1=city
    weather_call_r()
    return "设置城市1为"+city

@app.get("/weather/city2/{city}")
def set_city2(city: str):
    global b_city_2
    b_city_2=city
    weather_call()
    return "设置城市2为"+city

@app.get("/weather/city2_r/{city}")
def set_city2_r(city: str):
    global r_city_2
    r_city_2=city
    weather_call_r()
    return "设置城市2为"+city

@app.get("/notice/b/{notice}")
def notice_api(notice: str):
    if q_notice.full():
        q_notice.queue.clear()
    q_notice.put('api')
    client.publish('notice',payload=notice,qos=0)
    print("消息api")
    i=0
    global data_api
    while i<100:
        if data_api != '0':
            data = data_api
            data_api='0'
            return data
        time.sleep(0.1)
        i=i+1
    return '未发送成功'

@app.get("/notice/r/{notice}")
def notice_r_api(notice: str):
    if q_notice.full():
        q_notice.queue.clear()
    q_notice.put('api')
    client.publish('notice_r',payload=notice,qos=0)
    print("消息api")
    i=0
    global data_api
    while i<100:
        if data_api != '0':
            data = data_api
            data_api='0'
            return data
        time.sleep(0.1)
        i=i+1
    return '未发送成功'

def weather_call():
        url = "https://free-api.heweather.net/s6/weather/forecast"
        payload = {
            'location':b_city_1,
            'key':'0faac61cd15a4ac3bb0303dc3c0c80e9'
            }
        r = requests.get(url,params=payload)
        data = json.loads(r.text)
        code = jsonpath.jsonpath(data,'$..cond_code_d')
        wea = jsonpath.jsonpath(data,'$..cond_txt_d')
        tmp_min = jsonpath.jsonpath(data,'$..tmp_min')
        tmp_max = jsonpath.jsonpath(data,'$..tmp_max')
        weather_dic['code_1']=code_dic[code[0]]
        weather_dic['wea_1']=wea[0]
        weather_dic['tmp_1_min']=tmp_min[0]
        weather_dic['tmp_1_max']=tmp_max[0]
        payload = {
            'location':b_city_2,
            'key':'0faac61cd15a4ac3bb0303dc3c0c80e9'
            }
        r = requests.get(url,params=payload)
        data = json.loads(r.text)
        code = jsonpath.jsonpath(data,'$..cond_code_d')
        wea = jsonpath.jsonpath(data,'$..cond_txt_d')
        tmp_min = jsonpath.jsonpath(data,'$..tmp_min')
        tmp_max = jsonpath.jsonpath(data,'$..tmp_max')
        weather_dic['code_2']=code_dic[code[0]]
        weather_dic['wea_2']=wea[0]
        weather_dic['tmp_2_min']=tmp_min[0]
        weather_dic['tmp_2_max']=tmp_max[0]
        weather_json = json.dumps(weather_dic,ensure_ascii=False)
        print(weather_json)
        client.publish('weather',payload=weather_json,qos=0)

def weather_call_r():
        url = "https://free-api.heweather.net/s6/weather/forecast"
        payload = {
            'location':r_city_1,
            'key':'0faac61cd15a4ac3bb0303dc3c0c80e9'
            }
        r = requests.get(url,params=payload)
        data = json.loads(r.text)
        code = jsonpath.jsonpath(data,'$..cond_code_d')
        wea = jsonpath.jsonpath(data,'$..cond_txt_d')
        tmp_min = jsonpath.jsonpath(data,'$..tmp_min')
        tmp_max = jsonpath.jsonpath(data,'$..tmp_max')
        weather_dic_r['code_1']=code_dic[code[0]]
        weather_dic_r['wea_1']=wea[0]
        weather_dic_r['tmp_1_min']=tmp_min[0]
        weather_dic_r['tmp_1_max']=tmp_max[0]
        payload = {
            'location':r_city_2,
            'key':'0faac61cd15a4ac3bb0303dc3c0c80e9'
            }
        r = requests.get(url,params=payload)
        data = json.loads(r.text)
        code = jsonpath.jsonpath(data,'$..cond_code_d')
        wea = jsonpath.jsonpath(data,'$..cond_txt_d')
        tmp_min = jsonpath.jsonpath(data,'$..tmp_min')
        tmp_max = jsonpath.jsonpath(data,'$..tmp_max')
        weather_dic_r['code_2']=code_dic[code[0]]
        weather_dic_r['wea_2']=wea[0]
        weather_dic_r['tmp_2_min']=tmp_min[0]
        weather_dic_r['tmp_2_max']=tmp_max[0]
        weather_json = json.dumps(weather_dic_r,ensure_ascii=False)
        print(weather_json)
        client.publish('weather_r',payload=weather_json,qos=0)

# def temperature_lock():
#     while True:
#         data = q.get(block=True,timeout=None)
#         send_to_uesr('854214642',"当前温度:"+data)

#mqtt 连接成功回调函数
def on_connect(client, userdata, flags, rc):
    print("Connected with result code: " + str(rc))
#mqtt 接收信息成功回调函数
def on_message(client, userdata, msg):
    msg.payload = msg.payload.decode('utf-8')
    str_test=msg.topic + " " + str(msg.payload)
    print(str_test)
    global data_api
    if msg.topic == 'door' :
        send_to_uesr('854214642',"门的当前状态为:"+str(msg.payload))
    elif msg.topic == 'from_temperature':
        #send_to_uesr('854214642',"当前温度:"+str(msg.payload))
        #data = 854214642
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api="当前温度:"+str(float(str(msg.payload))/10)+'℃'
            else:
                send_to_uesr(data,"当前温度:"+str(float(str(msg.payload))/10)+'℃')
        print("当前温度:"+str(msg.payload)+'℃')
    elif msg.topic == 'from_humidity':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api="当前湿度:"+str(msg.payload)+'%'
            else:
                send_to_uesr(data,"当前湿度:"+str(msg.payload)+'%')
        print("当前湿度:"+str(msg.payload)+'%')
    elif msg.topic == 'from_iaq':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api="当前空气指数:"+str(msg.payload)
            else:
                send_to_uesr(data,"当前空气指数:"+str(msg.payload))
        print("当前空气指数:"+str(msg.payload))
    elif msg.topic == 'from_pressure':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api="当前气压:"+str(msg.payload)+'mPa'
            else:
                send_to_uesr(data,"当前气压:"+str(msg.payload)+'mPa')
        print("当前气压:"+str(msg.payload)+'mPa')
    elif msg.topic == 'notice_back':
        if not q_notice.empty():
            data = q_notice.get()
            if data == 'api':
                data_api="已经发送成功了吼吼"
            else:
                send_to_uesr(data,"已经发送成功了吼吼")
    elif msg.topic == 'from_temperature_r':
        #send_to_uesr('854214642',"当前温度:"+str(msg.payload))
        #data = 854214642
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api="当前温度:"+str(float(str(msg.payload))/10)+'℃'
            else:
                send_to_uesr(data,"当前温度:"+str(float(str(msg.payload))/10)+'℃')
        print("当前温度:"+str(msg.payload)+'℃')
    elif msg.topic == 'from_humidity_r':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api="当前湿度:"+str(msg.payload)+'%'
            else:
                send_to_uesr(data,"当前湿度:"+str(msg.payload)+'%')
        print("当前湿度:"+str(msg.payload)+'%')
    elif msg.topic == 'from_pressure_r':
        if not q.empty():
            data = q.get()
            if data == 'api':
                data_api="当前气压:"+str(msg.payload)+'mPa'
            else:
                send_to_uesr(data,"当前气压:"+str(msg.payload)+'mPa')
        print("当前气压:"+str(msg.payload)+'mPa')
    elif msg.topic == 'notice_back_r':
        if not q_notice.empty():
            data = q_notice.get()
            if data == 'api':
                data_api="已经发送成功了吼吼"
            else:
                send_to_uesr(data,"已经发送成功了吼吼")
    elif msg.topic == 'weather_call':
        weather_call()
    elif msg.topic == 'weather_call_r':
        weather_call_r()
#ws接收到信息回调函数
def ws_on_message(ws, message):
    #print(ws)
    #print(message)
    text = json.loads(message) #json解析
    print(text)
    if 'message' in text :
        print('message:'+text['message'])
        print('QQ:'+str(text['sender']['user_id']))
        if str(text['user_id']) in admin_user:
            #print("你是超级管理员")
            if re.search('开灯',text['message'],re.I):
                client.publish('to_led',payload='1',qos=0)
                send_to_uesr(str(text['user_id']),"好的，灯打开了！")
                print("开灯")
            elif re.search('关灯',text['message'],re.I):
                client.publish('to_led',payload='0',qos=0)
                send_to_uesr(str(text['user_id']),"好的，灯关闭了！")
                print("关灯")
            elif re.search('开打印机',text['message'],re.I):
                client.publish('to_printer',payload='1',qos=0)
                send_to_uesr(str(text['user_id']),"好的，打印机打开了！")
                print("开打印机")
            elif re.search('关打印机',text['message'],re.I):
                client.publish('to_printer',payload='0',qos=0)
                send_to_uesr(str(text['user_id']),"好的，打印机关闭了！")
                print("关打印机")
            elif text['message'][0]=='b': 
                if q_notice.full():
                    q_notice.queue.clear()
                q_notice.put(str(text['user_id']))
                client.publish('notice',payload=text['message'][1:],qos=0)
            elif text['message'][0]=='r': 
                if q_notice.full():
                    q_notice.queue.clear()
                q_notice.put(str(text['user_id']))
                client.publish('notice_r',payload=text['message'][1:],qos=0)
            elif re.search('冉',text['message'],re.I):
                if re.search('温度',text['message'],re.I):
                    if q.full():
                        q.queue.clear()
                    q.put(str(text['user_id']))
                    client.publish('to_temperature_r',payload='1',qos=0)
                    print("温度")
                elif re.search('湿度',text['message'],re.I):
                    if q.full():
                        q.queue.clear()
                    q.put(str(text['user_id']))
                    client.publish('to_humidity_r',payload='1',qos=0)
                    print("湿度")
                elif re.search('气压',text['message'],re.I):
                    if q.full():
                        q.queue.clear()
                    q.put(str(text['user_id']))
                    client.publish('to_pressure_r',payload='1',qos=0)
                    print("气压")
            elif re.search('宝',text['message'],re.I):
                if re.search('温度',text['message'],re.I):
                    if q.full():
                        q.queue.clear()
                    q.put(str(text['user_id']))
                    client.publish('to_temperature',payload='1',qos=0)
                    print("温度")
                elif re.search('湿度',text['message'],re.I):
                    if q.full():
                        q.queue.clear()
                    q.put(str(text['user_id']))
                    client.publish('to_humidity',payload='1',qos=0)
                    print("湿度")
                elif re.search('气压',text['message'],re.I):
                    if q.full():
                        q.queue.clear()
                    q.put(str(text['user_id']))
                    client.publish('to_pressure',payload='1',qos=0)
                    print("气压")
                elif re.search('空气',text['message'],re.I):
                    if q.full():
                        q.queue.clear()
                    q.put(str(text['user_id']))
                    client.publish('to_iaq',payload='1',qos=0)
                    print("空气")


#ws错误回调函数
def ws_on_error(ws, error):
    print(ws)
    print(error)
#ws关闭连接回调函数
def ws_on_close(ws):
    print(ws)
    print("### closed ###")
#ws打开连接回调函数    
def ws_on_open(ws):
    my_ws.send(str_1,1)

#ws发送给用户
def send_to_uesr(user,message):
    my_ws.send("{\"action\": \"send_private_msg\",\"params\": {\"user_id\": "+user+",\"message\": \""+message+"\"}}")        

#ws 初始化函数
def ws_init():
    #my_ws = websocket.WebSocketApp("ws://127.0.0.1:6700",on_message=on_message,on_error=on_error,on_close=on_close,on_open=on_open)
    my_ws.on_message=ws_on_message
    my_ws.on_error=ws_on_error
    my_ws.on_close=ws_on_close
    my_ws.on_open=ws_on_open
    my_ws.run_forever()
#mqtt 初始化函数    
def mqtt_init():
    time.sleep(0.2)
    client.on_connect = on_connect
    client.on_message = on_message
    #client.on_disconnect = on_disconnect
    client.connect('127.0.0.1', 1883, 600) # 600为keepalive的时间间隔
    client.subscribe('from_temperature', qos=0)
    client.subscribe('weather_call',qos=0)
    client.subscribe('notice_back',qos=0)
    client.subscribe('from_humidity',qos=0)
    client.subscribe('from_iaq',qos=0)
    client.subscribe('from_pressure',qos=0)
    client.subscribe('from_temperature_r', qos=0)
    client.subscribe('weather_call_r',qos=0)
    client.subscribe('notice_back_r',qos=0)
    client.subscribe('from_humidity_r',qos=0)
    #client.subscribe('from_iaq',qos=0)
    client.subscribe('from_pressure_r',qos=0)
    client.loop_forever() # 保持连接


def main() :
    t1= threading.Thread(target=ws_init)
    t2= threading.Thread(target=mqtt_init)
    #t3= threading.Thread(target=temperature_lock)
    t1.start()
    t2.start()
    #t3.start()

if __name__ == '__main__':
    main()
    uvicorn.run(app=app,host="0.0.0.0",port=8899,log_level="info")
    
