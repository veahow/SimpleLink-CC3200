# -*- coding:utf-8 -*-

import hashlib
import socket
import threading
import time
import xml.etree.ElementTree as ET
import re

from flask import Flask, make_response, request

app = Flask(__name__)

BUF_SIZE = 1400
socket_status = 0 # 连接:1 未连接:0

def tcplink():
#     print('Accept new connection from %s:%s...' % addrCli)
#     socket_status = 1
#     while True:
#         data = sockCli.recv(BUF_SIZE)
#         print(data)
#         data = data.decode('utf-8')
#         time.sleep(1)
#         if not data or data[0:4] == 'exit':
#             break
#         print(data)
    
# #     # while True:
# #     #     data = sockCli.recv(BUF_SIZE)
# #     #     temp = data.decode('utf-8')[0:1]
# #     #     # 这边肯定有问题 需要更改
# #     #     data_status = 1
# #     #     time.sleep(1)
# #         #sock.send(('Hello, %s!' % data.decode('utf-8')).encode('utf-8'))
# #     # while True:
# #     #     data = input('Please Input:')
# #     #     sock.send(data.encode(encoding='utf-8'))
# #     #     time.sleep(1)
#     sockCli.close()
#     print('Connection from %s:%s closed.' % addrCli)
    pass


@app.route("/")
def index():
    pass

@app.route("/cc3200", methods=['GET', 'POST'])
def wechat():
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
                    if socket_status == 1:
                        # sockCli.send('led_open')
                        # data = sockCli.recv(BUF_SIZE)
                        # content = data.decode('utf-8')[0:1]
                        pass


                if event_key == 'led_close':    # LED关
                    pass
                if event_key == 'led_status':    # LED当前状态
                    pass


                # 温度
                if event_key == 'tmp_status':    # 温度状态
                    pass

                # PWM
                if event_key == 'pwm_add':    # 增加PWM
                    pass

                if event_key == 'pwm_reduce':    # 减少PWM
                    pass

                if event_key == 'pwm_status':    # PWM当前状态 占空比
                    pass
        else:
            content = xml_rec.find('Content').text
        response = make_response(xml_rep % (fromu, tou, str(int(time.time())), content))
        response.content_type='application/xml'
        return response


if __name__ == "__main__":
    sockServ = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # host = socket.gethostname()
    # print(host)
    # port = 8032    # nginx转发8000端口的数据流到8030
    # sockServ.bind(('127.0.0.1', port))
    while True:
        data = input()
        sockServ.sendto(data.encode(encoding='utf-8'), ('118.25.105.240', 8033))

        # 创建新线程来处理TCP连接:
        # t = threading.Thread(target=tcplink)
        # t.start()
    # t = threading.Thread(target=tcplink)
    # t.start()

    # app.run(host='0.0.0.0', port=443)
