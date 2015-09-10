#  
# function a_handler() returns nothing
# 	echo "First handler"
# end
# 
# function a_handler2(v as int) returns nothing
# 	echo "Second handler: ", v
# end
# 
# function a_handler3(o, v as int) returns nothing
# 	echo "Third handler: '", o, "', ", v
# end
# 
# a_event=geta().event
# a_event.register(a_handler)
# token=a_event.register(a_handler2)
# a_event.register(a_handler3)
# a_event.fire(3)
# a_event.unregister(token)
# a_event.fire(7)
# 
# unset a_handler
# 
# a_event.fire(4)
#
# ## OUTPUT ##
# First handler
# Second handler: 3
# Third handler: '', 3
# First handler
# Third handler: '', 7
# First handler
# Third handler: '', 4
