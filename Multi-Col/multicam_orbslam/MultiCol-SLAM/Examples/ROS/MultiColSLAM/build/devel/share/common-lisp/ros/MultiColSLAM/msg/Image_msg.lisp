; Auto-generated. Do not edit!


(cl:in-package MultiColSLAM-msg)


;//! \htmlinclude Image_msg.msg.html

(cl:defclass <Image_msg> (roslisp-msg-protocol:ros-message)
  ((Img1
    :reader Img1
    :initarg :Img1
    :type sensor_msgs-msg:Image
    :initform (cl:make-instance 'sensor_msgs-msg:Image))
   (Img2
    :reader Img2
    :initarg :Img2
    :type sensor_msgs-msg:Image
    :initform (cl:make-instance 'sensor_msgs-msg:Image))
   (Img3
    :reader Img3
    :initarg :Img3
    :type sensor_msgs-msg:Image
    :initform (cl:make-instance 'sensor_msgs-msg:Image))
   (stamp
    :reader stamp
    :initarg :stamp
    :type cl:float
    :initform 0.0))
)

(cl:defclass Image_msg (<Image_msg>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <Image_msg>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'Image_msg)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name MultiColSLAM-msg:<Image_msg> is deprecated: use MultiColSLAM-msg:Image_msg instead.")))

(cl:ensure-generic-function 'Img1-val :lambda-list '(m))
(cl:defmethod Img1-val ((m <Image_msg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader MultiColSLAM-msg:Img1-val is deprecated.  Use MultiColSLAM-msg:Img1 instead.")
  (Img1 m))

(cl:ensure-generic-function 'Img2-val :lambda-list '(m))
(cl:defmethod Img2-val ((m <Image_msg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader MultiColSLAM-msg:Img2-val is deprecated.  Use MultiColSLAM-msg:Img2 instead.")
  (Img2 m))

(cl:ensure-generic-function 'Img3-val :lambda-list '(m))
(cl:defmethod Img3-val ((m <Image_msg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader MultiColSLAM-msg:Img3-val is deprecated.  Use MultiColSLAM-msg:Img3 instead.")
  (Img3 m))

(cl:ensure-generic-function 'stamp-val :lambda-list '(m))
(cl:defmethod stamp-val ((m <Image_msg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader MultiColSLAM-msg:stamp-val is deprecated.  Use MultiColSLAM-msg:stamp instead.")
  (stamp m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <Image_msg>) ostream)
  "Serializes a message object of type '<Image_msg>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'Img1) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'Img2) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'Img3) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'stamp))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <Image_msg>) istream)
  "Deserializes a message object of type '<Image_msg>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'Img1) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'Img2) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'Img3) istream)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'stamp) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<Image_msg>)))
  "Returns string type for a message object of type '<Image_msg>"
  "MultiColSLAM/Image_msg")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'Image_msg)))
  "Returns string type for a message object of type 'Image_msg"
  "MultiColSLAM/Image_msg")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<Image_msg>)))
  "Returns md5sum for a message object of type '<Image_msg>"
  "e714c77d6e3064511cd0b1892db2ecc2")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'Image_msg)))
  "Returns md5sum for a message object of type 'Image_msg"
  "e714c77d6e3064511cd0b1892db2ecc2")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<Image_msg>)))
  "Returns full string definition for message of type '<Image_msg>"
  (cl:format cl:nil "sensor_msgs/Image Img1~%sensor_msgs/Image Img2~%sensor_msgs/Image Img3~%float32 stamp~%~%================================================================================~%MSG: sensor_msgs/Image~%# This message contains an uncompressed image~%# (0, 0) is at top-left corner of image~%#~%~%Header header        # Header timestamp should be acquisition time of image~%                     # Header frame_id should be optical frame of camera~%                     # origin of frame should be optical center of cameara~%                     # +x should point to the right in the image~%                     # +y should point down in the image~%                     # +z should point into to plane of the image~%                     # If the frame_id here and the frame_id of the CameraInfo~%                     # message associated with the image conflict~%                     # the behavior is undefined~%~%uint32 height         # image height, that is, number of rows~%uint32 width          # image width, that is, number of columns~%~%# The legal values for encoding are in file src/image_encodings.cpp~%# If you want to standardize a new string format, join~%# ros-users@lists.sourceforge.net and send an email proposing a new encoding.~%~%string encoding       # Encoding of pixels -- channel meaning, ordering, size~%                      # taken from the list of strings in include/sensor_msgs/image_encodings.h~%~%uint8 is_bigendian    # is this data bigendian?~%uint32 step           # Full row length in bytes~%uint8[] data          # actual matrix data, size is (step * rows)~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%# 0: no frame~%# 1: global frame~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'Image_msg)))
  "Returns full string definition for message of type 'Image_msg"
  (cl:format cl:nil "sensor_msgs/Image Img1~%sensor_msgs/Image Img2~%sensor_msgs/Image Img3~%float32 stamp~%~%================================================================================~%MSG: sensor_msgs/Image~%# This message contains an uncompressed image~%# (0, 0) is at top-left corner of image~%#~%~%Header header        # Header timestamp should be acquisition time of image~%                     # Header frame_id should be optical frame of camera~%                     # origin of frame should be optical center of cameara~%                     # +x should point to the right in the image~%                     # +y should point down in the image~%                     # +z should point into to plane of the image~%                     # If the frame_id here and the frame_id of the CameraInfo~%                     # message associated with the image conflict~%                     # the behavior is undefined~%~%uint32 height         # image height, that is, number of rows~%uint32 width          # image width, that is, number of columns~%~%# The legal values for encoding are in file src/image_encodings.cpp~%# If you want to standardize a new string format, join~%# ros-users@lists.sourceforge.net and send an email proposing a new encoding.~%~%string encoding       # Encoding of pixels -- channel meaning, ordering, size~%                      # taken from the list of strings in include/sensor_msgs/image_encodings.h~%~%uint8 is_bigendian    # is this data bigendian?~%uint32 step           # Full row length in bytes~%uint8[] data          # actual matrix data, size is (step * rows)~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%# 0: no frame~%# 1: global frame~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <Image_msg>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'Img1))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'Img2))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'Img3))
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <Image_msg>))
  "Converts a ROS message object to a list"
  (cl:list 'Image_msg
    (cl:cons ':Img1 (Img1 msg))
    (cl:cons ':Img2 (Img2 msg))
    (cl:cons ':Img3 (Img3 msg))
    (cl:cons ':stamp (stamp msg))
))
