# -*- coding:utf-8 -*-

import hashlib
import socket
import threading
import time
import xml.etree.ElementTree as ET
import re

from flask import Flask, render_template, make_response, request

app = Flask(__name__)

BUF_SIZE = 1400
port = 8030
socket_status = 0 # 连接:1 未连接:0
# sockServ = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sockServ = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP
sockServ.bind(('0.0.0.0', port))
sockServ.listen(5)
print('Waiting for connection...')

def tcplink():
    global sockCli
    global addrCli
    global socket_status
    sockCli, addrCli = sockServ.accept()
    print('Accept new connection from %s:%s...' % addrCli)
    socket_status = 1
    # while True:
    #     data = sockCli.recv(BUF_SIZE)
    #     print(data)
    #     data = data.decode('utf-8')
    #     time.sleep(1)
    #     if not data or data[0:4] == 'exit':
    #         break
    #     print(data)
    # while True:
    #     data = sockCli.recv(BUF_SIZE)
    #     temp = data.decode('utf-8')[0:1]
    #     # 这边肯定有问题 需要更改
    #     data_status = 1
    #     time.sleep(1)
        #sock.send(('Hello, %s!' % data.decode('utf-8')).encode('utf-8'))
    # while True:
    #     data = input('Please Input:')
    #     sock.send(data.encode(encoding='utf-8'))
    #     time.sleep(1)
    # sockCli.close()
    # print('Connection from %s:%s closed.' % addrCli)

def udplink():
    global socket_status
    global sockServ
    global addrCli
    while True:
        print('Waiting for udp request of CC3200')    # 等待CC3200发送数据到服务器
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


@app.route("/")
def index():
    return render_template('index.html')

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
                    # TCP
                    if socket_status == 1:
                        sockCli.send('led_open'.encode(encoding='utf-8'))    # 发送请求
                        # if not data or data[0:4] == 'exit':
                        # 必须加错误处理程序
                        content = 'LED开'
                    else:
                        content = 'CC3200与服务器未连接'

                    # UDP
                    # if socket_status == 1:
                    #     sockServ.sendto('led_open'.encode(encoding='utf-8'), addrCli)
                    #     time.sleep(1)
                    #     content = 'LED开'
                    # else:
                    #     content = 'CC3200与服务器未连接'


                if event_key == 'led_close':    # LED关
                    # TCP
                    if socket_status == 1:
                        sockCli.send('led_close'.encode(encoding='utf-8'))    # 发送请求
                        content = 'LED关'
                    else:
                        content = 'CC3200与服务器未连接'

                    # UDP
                    # if socket_status == 1:
                    #     sockServ.sendto('led_close'.encode(encoding='utf-8'), addrCli)
                    #     time.sleep(1)
                    #     content = 'LED关'
                    # else:
                    #     content = 'CC3200与服务器未连接'

                    
                if event_key == 'led_status':    # LED当前状态
                    # TCP
                    if socket_status == 1:
                        sockCli.send('led_status'.encode(encoding='utf-8'))    # 发送请求
                        led_status = sockCli.recv(BUF_SIZE)
                        time.sleep(1)
                        led_status = led_status.decode('utf-8')[0:1]    # CC3200传输的内容
                        if led_status == '1':
                            content = '当前CC3200 LED开'
                        if led_status == '0':
                            content = '当前CC3200 LED关'
                    else:
                        content = 'CC3200与服务器未连接'

                    # UDP
                    # if socket_status == 1:
                    #     sockServ.sendto('led_status'.encode(encoding='utf-8'), addrCli)
                    #     led_status, addr = sockServ.recvfrom(BUF_SIZE)
                    #     led_status = led_status.decode('utf-8')[0:1]    # CC3200传输的内容
                    #     time.sleep(1)
                    #     if led_status == '1':
                    #         content = '当前CC3200 LED开'
                    #     if led_status == '0':
                    #         content = '当前CC3200 LED关'
                    # else:
                    #     content = 'CC3200与服务器未连接'


                # 温度
                if event_key == 'tmp_status':    # 温度状态
                    # TCP
                    if socket_status == 1:
                        sockCli.send('tmp_status'.encode(encoding='utf-8'))
                        tmp_int = sockCli.recv(BUF_SIZE)
                        tmp_int = tmp_int.decode('utf-8')[0:3]    # 整数位

                        tmp_dec = sockCli.recv(BUF_SIZE)
                        tmp_dec = tmp_dec.decode('utf-8')[0:2]    # 小数位

                        tmp_status = tmp_int + tmp_dec

                        content = '当前温度值:%s度' % tmp_status
                    else:
                        content = 'CC3200与服务器未连接'

                    # UDP
                    # if socket_status == 1:
                    #     sockServ.sendto('tmp_status'.encode(encoding='utf-8'), addrCli)
                    #     tmp_status, addr = sockServ.recvfrom(BUF_SIZE)
                    #     tmp_status = tmp_status.decode('utf-8')[0:5]    # CC3200传输的内容
                    #     time.sleep(1)
                    #     content = '当前温度值:%s℃' % tmp_status
                    # else:
                    #     content = 'CC3200与服务器未连接'

                # 湿度
                if event_key == 'hum_status':    # 湿度状态
                    content = '该功能未开发'

                # PWM
                if event_key == 'pwm_add':    # 增加PWM
                    content = '该功能未开发'

                if event_key == 'pwm_reduce':    # 减少PWM
                    content = '该功能未开发'

                if event_key == 'pwm_status':    # PWM当前状态 占空比
                    content = '该功能未开发'
        else:
            content = xml_rec.find('Content').text

        response = make_response(xml_rep % (fromu, tou, str(int(time.time())), content))
        response.content_type='application/xml'
        return response


if __name__ == "__main__":
    # sockServ = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # host = socket.gethostname()
    # port = 8030
    # sockServ.bind((host, port))
    # sockServ.listen(5)
    # print('Waiting for connection...')
    # while True:
    #     # 接受一个新连接:
    #     sockCli, addrCli = sockServ.accept()
    #     # 创建新线程来处理TCP连接:
    #     t = threading.Thread(target=tcplink, args=(sockCli, addrCli))
    #     t.start()
    t = threading.Thread(target=tcplink)
    # t = threading.Thread(target=udplink)
    t.start()

    # 启动flask
    app.run(host='0.0.0.0', port=80)
