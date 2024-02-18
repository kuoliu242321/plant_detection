var mqtt = require('../../utils/mqtt.min.js');
const crypto = require('../../utils/hex_hmac_sha1.js');

Page({
  data: {
    client: null, // 将 client 定义在 data 中
    temperaturevalue: "",
    humidityvalue: "",
    airPressurevalue: "",
    soilHumidityvalue: "",
    inputValue: ''
  },

  onLoad: function () {
    // 注意：这里在程序运行后会直接进行连接，如果你要真机调试，记得关掉模拟器或者使用一个按钮来控制连接
    this.doConnect();
  },

  doConnect() {
    const deviceConfig = {
      productKey: "xxxxxx",
      deviceName: "xxxxxxx",
      deviceSecret: "xxxxxxxxxxxxxxxxxxxx",
      regionId: "cn-shanghai"
    };

    const options = this.initMqttOptions(deviceConfig);
    console.log(options);

    // 替换 productKey 为你自己的产品的（注意这里是 wxs，不是 wss，否则你可能会碰到 ws 不是构造函数的错误）
    this.setData({
      client: mqtt.connect('wxs://k0sz1LKs4CN.iot-as-mqtt.cn-shanghai.aliyuncs.com', options)
    });

    this.data.client.on('connect', () => {
      console.log('连接服务器成功');
      this.data.client.subscribe('/sys/k0sz1LKs4CN/mobile_app/thing/event/property/post_reply', (err) => {
        if (!err) {
          console.log('订阅成功！');
        }
      });
    });

    this.data.client.on('message', (topic, message) => {
      // message is Buffer
      let msg = message.toString();
      // console.log('收到消息：'+msg);
      //////////////////////////////////////////////////////////////////////////
      processReceivedMessage(msg, this);
    });
  },

  // IoT 平台 MQTT 连接参数初始化
  initMqttOptions(deviceConfig) {
    const params = {
      productKey: deviceConfig.productKey,
      deviceName: deviceConfig.deviceName,
      timestamp: Date.now(),
      clientId: Math.random().toString(36).substr(2),
    };

    // CONNECT 参数
    const options = {
      keepalive: 60, // 60s
      clean: true, // cleanSession 不保持持久会话
      protocolVersion: 4 // MQTT v3.1.1
    };

    // 1.生成 clientId，username，password
    options.password = this.signHmacSha1(params, deviceConfig.deviceSecret);
    options.clientId = `${params.clientId}|securemode=2,signmethod=hmacsha1,timestamp=${params.timestamp}|`;
    options.username = `${params.deviceName}&${params.productKey}`;

    return options;
  },


  signHmacSha1(params, deviceSecret) {
    let keys = Object.keys(params).sort();
    // 按字典序排序
    keys = keys.sort();
    const list = [];
    keys.map((key) => {
      list.push(`${key}${params[key]}`);
    });
    const contentStr = list.join('');
    return crypto.hex_hmac_sha1(deviceSecret, contentStr);
  },
  
  onPublishClick: function (event) {
    // 获取输入框的值
    const inputValue = this.data.inputValue;

    // 检查输入是否为空
    if (!inputValue) {
      wx.showToast({
        title: '请输入有效的值',
        icon: 'none',
        duration: 2000
      });
      return;
    }

    // 检查输入是否为1到99的整数
    const intValue = parseInt(inputValue);
    if (isNaN(intValue) || intValue < 1 || intValue > 99) {
      wx.showToast({
        title: '请输入1到99的整数',
        icon: 'none',
        duration: 2000
      });
      return;
    }

    // 在这里添加发布消息的逻辑
    this.publishMessage(inputValue);
  },

  publishMessage(code) {
    // 确保客户端对象存在
    if (!this.data.client) {
      console.error('MQTT 客户端未初始化');
      return;
    }
  
    // 构造发布消息的格式
    const publishData = {
      params: {
        code: code
      },
      version: "1.0.0"
    };
  
    // 将对象转换为 JSON 字符串
    const jsonString = JSON.stringify(publishData);
  
    // 发布消息到指定主题
    this.data.client.publish('/sys/k0sz1LKs4CN/mobile_app/thing/event/property/post', jsonString);
    console.log('post successful!');
    console.log(jsonString);
  },

  // 在输入框的 change 事件中更新 inputValue
  onCodeInput: function (event) {
    const inputValue = event.detail.value || ''; // 如果为 undefined，则将其转换为空字符串
    this.setData({
      inputValue: inputValue
      
    });
  },
});

function processReceivedMessage(message, page) {
    //console.log('Received Message:', message);
    var parsedMessage = JSON.parse(message);
  
    // 添加对 parsedMessage.items 的存在性检查
    if (parsedMessage && parsedMessage.items) {
      // 提取信息
      var device_Type = parsedMessage.deviceType;
      var iot_Id = parsedMessage.iotId;
      var request_Id = parsedMessage.requestId;
      // ... 其他信息的提取
      // 提取 items 中的温度、湿度、大气压力和土壤湿度值
      var temperature = parsedMessage.items.temp ? parsedMessage.items.temp.value : null;
      var humidity = parsedMessage.items.humity.value;
      var airPressure = parsedMessage.items.Airpressure ? parsedMessage.items.Airpressure.value : null;
      var soilHumidity = parsedMessage.items.soil_humity.value;
      // 打印提取的信息
      console.log('Received Message:', parsedMessage); 
      console.log('Device Type:', device_Type);
      console.log('IoT ID:', iot_Id);
      console.log('Request ID:', request_Id);
      console.log('Temperature Value:', temperature);
      console.log('Humidity Value:', humidity);
      console.log('Air Pressure Value:', airPressure);
      console.log('Soil Humidity Value:', soilHumidity);
      // 更新页面数据
      page.setData({
        temperaturevalue: temperature,
        humidityvalue: humidity,
        airPressurevalue: airPressure,
        soilHumidityvalue: soilHumidity
      });
    } else {
      console.log('Received Message format error:', parsedMessage);
    }
  }