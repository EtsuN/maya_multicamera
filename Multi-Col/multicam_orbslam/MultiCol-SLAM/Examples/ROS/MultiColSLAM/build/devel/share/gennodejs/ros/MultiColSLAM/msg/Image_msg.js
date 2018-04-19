// Auto-generated. Do not edit!

// (in-package MultiColSLAM.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let sensor_msgs = _finder('sensor_msgs');

//-----------------------------------------------------------

class Image_msg {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.Img1 = null;
      this.Img2 = null;
      this.Img3 = null;
      this.stamp = null;
    }
    else {
      if (initObj.hasOwnProperty('Img1')) {
        this.Img1 = initObj.Img1
      }
      else {
        this.Img1 = new sensor_msgs.msg.Image();
      }
      if (initObj.hasOwnProperty('Img2')) {
        this.Img2 = initObj.Img2
      }
      else {
        this.Img2 = new sensor_msgs.msg.Image();
      }
      if (initObj.hasOwnProperty('Img3')) {
        this.Img3 = initObj.Img3
      }
      else {
        this.Img3 = new sensor_msgs.msg.Image();
      }
      if (initObj.hasOwnProperty('stamp')) {
        this.stamp = initObj.stamp
      }
      else {
        this.stamp = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type Image_msg
    // Serialize message field [Img1]
    bufferOffset = sensor_msgs.msg.Image.serialize(obj.Img1, buffer, bufferOffset);
    // Serialize message field [Img2]
    bufferOffset = sensor_msgs.msg.Image.serialize(obj.Img2, buffer, bufferOffset);
    // Serialize message field [Img3]
    bufferOffset = sensor_msgs.msg.Image.serialize(obj.Img3, buffer, bufferOffset);
    // Serialize message field [stamp]
    bufferOffset = _serializer.float32(obj.stamp, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type Image_msg
    let len;
    let data = new Image_msg(null);
    // Deserialize message field [Img1]
    data.Img1 = sensor_msgs.msg.Image.deserialize(buffer, bufferOffset);
    // Deserialize message field [Img2]
    data.Img2 = sensor_msgs.msg.Image.deserialize(buffer, bufferOffset);
    // Deserialize message field [Img3]
    data.Img3 = sensor_msgs.msg.Image.deserialize(buffer, bufferOffset);
    // Deserialize message field [stamp]
    data.stamp = _deserializer.float32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += sensor_msgs.msg.Image.getMessageSize(object.Img1);
    length += sensor_msgs.msg.Image.getMessageSize(object.Img2);
    length += sensor_msgs.msg.Image.getMessageSize(object.Img3);
    return length + 4;
  }

  static datatype() {
    // Returns string type for a message object
    return 'MultiColSLAM/Image_msg';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'e714c77d6e3064511cd0b1892db2ecc2';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    sensor_msgs/Image Img1
    sensor_msgs/Image Img2
    sensor_msgs/Image Img3
    float32 stamp
    
    ================================================================================
    MSG: sensor_msgs/Image
    # This message contains an uncompressed image
    # (0, 0) is at top-left corner of image
    #
    
    Header header        # Header timestamp should be acquisition time of image
                         # Header frame_id should be optical frame of camera
                         # origin of frame should be optical center of cameara
                         # +x should point to the right in the image
                         # +y should point down in the image
                         # +z should point into to plane of the image
                         # If the frame_id here and the frame_id of the CameraInfo
                         # message associated with the image conflict
                         # the behavior is undefined
    
    uint32 height         # image height, that is, number of rows
    uint32 width          # image width, that is, number of columns
    
    # The legal values for encoding are in file src/image_encodings.cpp
    # If you want to standardize a new string format, join
    # ros-users@lists.sourceforge.net and send an email proposing a new encoding.
    
    string encoding       # Encoding of pixels -- channel meaning, ordering, size
                          # taken from the list of strings in include/sensor_msgs/image_encodings.h
    
    uint8 is_bigendian    # is this data bigendian?
    uint32 step           # Full row length in bytes
    uint8[] data          # actual matrix data, size is (step * rows)
    
    ================================================================================
    MSG: std_msgs/Header
    # Standard metadata for higher-level stamped data types.
    # This is generally used to communicate timestamped data 
    # in a particular coordinate frame.
    # 
    # sequence ID: consecutively increasing ID 
    uint32 seq
    #Two-integer timestamp that is expressed as:
    # * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
    # * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
    # time-handling sugar is provided by the client library
    time stamp
    #Frame this data is associated with
    # 0: no frame
    # 1: global frame
    string frame_id
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new Image_msg(null);
    if (msg.Img1 !== undefined) {
      resolved.Img1 = sensor_msgs.msg.Image.Resolve(msg.Img1)
    }
    else {
      resolved.Img1 = new sensor_msgs.msg.Image()
    }

    if (msg.Img2 !== undefined) {
      resolved.Img2 = sensor_msgs.msg.Image.Resolve(msg.Img2)
    }
    else {
      resolved.Img2 = new sensor_msgs.msg.Image()
    }

    if (msg.Img3 !== undefined) {
      resolved.Img3 = sensor_msgs.msg.Image.Resolve(msg.Img3)
    }
    else {
      resolved.Img3 = new sensor_msgs.msg.Image()
    }

    if (msg.stamp !== undefined) {
      resolved.stamp = msg.stamp;
    }
    else {
      resolved.stamp = 0.0
    }

    return resolved;
    }
};

module.exports = Image_msg;
