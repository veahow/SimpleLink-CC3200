# -*- coding:utf-8 -*-

import hashlib
import socket
import threading
import time
import xml.etree.ElementTree as ET
import re

from flask import Flask, render_template, make_response, request

app = Flask(__name__)

BUF_SIZE = 10

# udp socket端口号 注意要在服务器安全组设置放通相关的端口
port = 8030
# led_port = 8031
# tmp_port = 8032
# hum_port = 8033
# pwm_port = 8034

socket_status = 0 # 连接:1 未连接:0

# udp socket创建
sockServ = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
# led_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP LED
# tmp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP 温度
# hum_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP 湿度
# pwm_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP PWM

# udp socket绑定
sockServ.bind(('0.0.0.0', port))
# led_sock.bind(('0.0.0.0', led_port))
# tmp_sock.bind(('0.0.0.0', tmp_port))
# hum_sock.bind(('0.0.0.0', hum_port))
# pwm_sock.bind(('0.0.0.0', pwm_port))
led_status = '1'
tmp_status = 0
hum_status = 0
pwm_status = 0
ax = 0
ay = 0
az = 0

addrCli = ()

def udplink():
    global socket_status
    global sockServ
    global addrCli
    while True:
        print('等待CC3200发送UDP请求')    # 等待CC3200发送数据到服务器
        data, addrCli = sockServ.recvfrom(BUF_SIZE)
        data = data.decode('utf-8')
        print(data[0:2])
        if(data[0:2] == 'wx'):
            socket_status = 1 # 收到CC3200
            print('Received from %s:%s.' % addrCli)
            # sockServ.sendto('Hello'.encode(encoding='utf-8'), addrCli)    # 这里需要注意和CC3200交互 写代码
            break
        else:
            socket_status = 0

# def UdpLedStatus():
#     global led_sock
#     global led_status
#     global led_addr
#     while True:
#         print('等待CC3200的LED状态数据...')
#         led_status, led_addr = led_sock.recvfrom(BUF_SIZE)
#         led_status = led_status.decode('utf-8')[0:1]
#         if led_status == '1':
#             print('LED状态:打开')
#         if led_status == '0':
#             print('LED状态:关闭')

# def UdpTmpStatus():
#     global tmp_sock
#     global tmp_status
#     global tmp_addr
#     while True:
#         print('等待CC3200的温度状态数据...')
#         tmp_int, tmp_addr = tmp_sock.recvfrom(BUF_SIZE)
#         tmp_int = tmp_int.decode('utf-8')[0:3]    # 整数位

#         tmp_dec, tmp_addr = tmp_sock.recvfrom(BUF_SIZE)  
#         tmp_dec = tmp_dec.decode('utf-8')[0:2]    # 小数位
        
#         tmp_status = tmp_int + tmp_dec
#         print('当前温度值:%s℃' % tmp_status)

# def UdpHumStatus():
#     global hum_sock
#     global hum_status
#     global hum_addr
#     while True:
#         print('等待CC3200的湿度状态数据...')
#         hum_int, hum_addr = hum_sock.recvfrom(BUF_SIZE)
#         hum_int = hum_int.decode('utf-8')[0:3]    # 整数位

#         hum_dec, hum_addr = hum_sock.recvfrom(BUF_SIZE)  
#         hum_dec = hum_dec.decode('utf-8')[0:2]    # 小数位
        
#         hum_status = hum_int + hum_dec
#         print('当前湿度值:%sRH%%' % hum_status)

# def UdpPwmStatus():
#     pass

def UdpRecvData():
    global addrCli
    global led_status
    global tmp_status
    global hum_status
    global pwm_status
    global ax
    global ay
    global az
    while True:
        print('等待CC3200发送数据...')
        led_status, addrCli = sockServ.recvfrom(BUF_SIZE)
        led_status = led_status.decode('utf-8', 'ignore')[0:1]
        if led_status == '1':
            print('LED状态:打开')
        if led_status == '0':
            print('LED状态:关闭')

        # SHT20温度、湿度测量
        # tmp_int, addrCli = sockServ.recvfrom(BUF_SIZE)
        # tmp_int = tmp_int.decode('utf-8', 'ignore')[0:3]    # 整数位
        # tmp_dec, addrCli = sockServ.recvfrom(BUF_SIZE)  
        # tmp_dec = tmp_dec.decode('utf-8', 'ignore')[0:2]    # 小数位
        # tmp_status = tmp_int + tmp_dec
        # print('当前温度值:%s℃' % tmp_status)
        
        # hum_int, addrCli = sockServ.recvfrom(BUF_SIZE)
        # hum_int = hum_int.decode('utf-8', 'ignore')[0:3]    # 整数位
        # hum_dec, addrCli = sockServ.recvfrom(BUF_SIZE)  
        # hum_dec = hum_dec.decode('utf-8', 'ignore')[0:2]    # 小数位
        # hum_status = hum_int + hum_dec
        # print('当前湿度值:%sRH%%' % hum_status)

        # print('\n')
            
        # CC3200板载传感器温度、加速度测量
        tmp_int, addrCli = sockServ.recvfrom(BUF_SIZE)
        tmp_int = tmp_int.decode('utf-8', 'ignore')[0:3]    # 整数位
        tmp_dec, addrCli = sockServ.recvfrom(BUF_SIZE)  
        tmp_dec = tmp_dec.decode('utf-8', 'ignore')[0:2]    # 小数位
        tmp_status = tmp_int + tmp_dec
        print('当前温度值:%s℃' % tmp_status)
        
        ax, addrCli = sockServ.recvfrom(BUF_SIZE)
        ax = ax.decode('utf-8', 'ignore')[0:3]    # x加速度
        ay, addrCli = sockServ.recvfrom(BUF_SIZE)
        ay = ay.decode('utf-8', 'ignore')[0:3]    # y加速度
        az, addrCli = sockServ.recvfrom(BUF_SIZE)
        az = az.decode('utf-8', 'ignore')[0:3]    # z加速度
        print('当前加速度值 x:%s y:%s z:%s' % (ax, ay, az))

        # pwm
        pwm_status, addrCli = sockServ.recvfrom(BUF_SIZE)
        pwm_status = pwm_status.decode('utf-8', 'ignore')[0:3]
        print('当前PWM占空比:%s' % pwm_status)

        print('\n')

@app.route("/")
def index():
    return render_template('index.html', led_status=led_status, tmp_status=tmp_status, hum_status=hum_status, pwm_status=pwm_status, ax=ax, ay=ay, az=az)

@app.route("/about")
def about():
    return render_template('about.html')

@app.route("/cc3200", methods=['GET', 'POST'])
def wechat():
    global socket_status
    global content
    # 与微信公众号验证
    if request.method == "GET":       # 判断请求方式是GET请求
        signature = request.args.get('signature')     # 获取携带的signature参数
        timestamp = request.args.get('timestamp')     # 获取携带的timestamp参数
        nonce = request.args.get('nonce')        # 获取携带的nonce参数
        echostr = request.args.get('echostr')         # 获取携带的echostr参数

        token = 'cc3200wx'     # 微信公众号的token

        # 进行字典排序
        data = [token, timestamp, nonce]
        data.sort()
        
        # 拼接成字符串
        temp = ''.join(data)

        # 进行sha1加密
        hashcode = hashlib.sha1(temp.encode('utf-8')).hexdigest()
        
        # 加密后的字符串可与signature对比，标识该请求来源于微信
        if hashcode == signature:
            return make_response(echostr)
    else:
        rec = request.stream.read()    # 接收消息
        xml_rec = ET.fromstring(rec)   # 处理为ET对象 便于XML解析

        # 提取XML文本信息
        tou = xml_rec.find('ToUserName').text
        fromu = xml_rec.find('FromUserName').text
        msgtype = xml_rec.find('MsgType').text
        print(msgtype)

        xml_rep = '''<xml>
                        <ToUserName><![CDATA[%s]]></ToUserName>
                        <FromUserName><![CDATA[%s]]></FromUserName>
                        <CreateTime>%s</CreateTime>
                        <MsgType><![CDATA[text]]></MsgType>
                        <Content><![CDATA[%s]]></Content>
                        <FuncFlag>0</FuncFlag>
                    </xml>'''

        if msgtype == 'event':    # 事件类型
            event = xml_rec.find('Event').text
            if event == 'subscribe':    # 订阅事件
                content = '欢迎关注中国石油大学（华东）测控1503陈奕浩毕业设计测试号'
            if event == 'CLICK':    # 菜单点击事件
                event_key = xml_rec.find('EventKey').text

                # LED
                if event_key == 'led_open':    # LED开
                    # UDP
                    if addrCli:
                        sockServ.sendto('led_open'.encode(encoding='utf-8'), addrCli)
                        time.sleep(1)
                        content = 'LED开'
                    else:
                        content = 'CC3200与服务器未连接'


                if event_key == 'led_close':    # LED关
                    # UDP
                    if addrCli:
                        sockServ.sendto('led_close'.encode(encoding='utf-8'), addrCli)
                        time.sleep(1)
                        content = 'LED关'
                    else:
                        content = 'CC3200与服务器未连接'

                    
                if event_key == 'led_status':    # LED当前状态
                    # UDP
                    if addrCli:
                        # sockServ.sendto('led_status'.encode(encoding='utf-8'), addrCli)
                        # led_status, addr = sockServ.recvfrom(BUF_SIZE)
                        # led_status = led_status.decode('utf-8')[0:1]    # CC3200传输的内容
                        # time.sleep(1)
                        if led_status == '1':
                            content = '当前CC3200 LED开'
                        if led_status == '0':
                            content = '当前CC3200 LED关'
                    else:
                        content = 'CC3200与服务器未连接'


                # 温度
                if event_key == 'tmp_status':    # 温度状态
                    # UDP
                    if addrCli:
                        # sockServ.sendto('tmp_status'.encode(encoding='utf-8'), addrCli)
                        # tmp_int, addr = sockServ.recvfrom(BUF_SIZE)
                        # tmp_int = tmp_int.decode('utf-8')[0:3]    # 整数位

                        # tmp_dec, addr = sockServ.recvfrom(BUF_SIZE)  
                        # tmp_dec = tmp_dec.decode('utf-8')[0:2]    # 小数位

                        # tmp_status = tmp_int + tmp_dec
                        content = '当前温度值:%s℃' % tmp_status
                    else:
                        content = 'CC3200与服务器未连接'

                # 湿度
                if event_key == 'hum_status':    # 湿度状态
                    # UDP
                    if addrCli:
                        # sockServ.sendto('hum_status'.encode(encoding='utf-8'), addrCli)
                        # hum_int, addr = sockServ.recvfrom(BUF_SIZE)
                        # hum_int = hum_int.decode('utf-8')[0:3]    # 整数位

                        # hum_dec, addr = sockServ.recvfrom(BUF_SIZE)  
                        # hum_dec = hum_dec.decode('utf-8')[0:2]    # 小数位

                        # hum_status = hum_int + hum_dec
                        content = '当前湿度值:%s%%' % hum_status
                    else:
                        content = 'CC3200与服务器未连接'

                # 加速度
                if event_key == 'accel_status':    # 湿度状态
                    # UDP
                    if addrCli:
                        # sockServ.sendto('hum_status'.encode(encoding='utf-8'), addrCli)
                        # hum_int, addr = sockServ.recvfrom(BUF_SIZE)
                        # hum_int = hum_int.decode('utf-8')[0:3]    # 整数位

                        # hum_dec, addr = sockServ.recvfrom(BUF_SIZE)  
                        # hum_dec = hum_dec.decode('utf-8')[0:2]    # 小数位

                        # hum_status = hum_int + hum_dec
                        content = '当前加速度值 x:%s y:%s z:%s' % (ax, ay, az)
                    else:
                        content = 'CC3200与服务器未连接'

                # PWM
                if event_key == 'pwm_add':    # 增加PWM
                    # UDP
                    if addrCli:
                        sockServ.sendto('pwm_add'.encode(encoding='utf-8'), addrCli)
                        time.sleep(1)
                        content = 'PWM占空比增加'
                    else:
                        content = 'CC3200与服务器未连接'

                if event_key == 'pwm_reduce':    # 减少PWM
                    # UDP
                    if addrCli:
                        sockServ.sendto('pwm_reduce'.encode(encoding='utf-8'), addrCli)
                        time.sleep(1)
                        content = 'PWM占空比减少'
                    else:
                        content = 'CC3200与服务器未连接'

                if event_key == 'pwm_status':    # PWM当前状态 占空比
                    # UDP
                    if addrCli:
                        content = '当前PWM值:%s' % pwm_status
                    else:
                        content = 'CC3200与服务器未连接'
        else:
            content = xml_rec.find('Content').text

        response = make_response(xml_rep % (fromu, tou, str(int(time.time())), content))
        response.content_type='application/xml'
        return response


if __name__ == "__main__":
    #t = threading.Thread(target=udplink)
    # t1 = threading.Thread(target=UdpLedStatus)    # LED
    # t2 = threading.Thread(target=UdpTmpStatus)    # 温度
    # t3 = threading.Thread(target=UdpHumStatus)    # 湿度
    # t4 = threading.Thread(target=UdpPwmStatus)    # PWM
    # t1.start()
    # t2.start()
    # t3.start()
    #t4.start()
    #t.start()
    t = threading.Thread(target=UdpRecvData)
    t.start()

    # 启动flask
    app.run(host='0.0.0.0', port=80)
