
(cl:in-package :asdf)

(defsystem "MultiColSLAM-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :sensor_msgs-msg
)
  :components ((:file "_package")
    (:file "Image_msg" :depends-on ("_package_Image_msg"))
    (:file "_package_Image_msg" :depends-on ("_package"))
  ))