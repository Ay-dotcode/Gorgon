#  
# function a_handler(v as int) returns nothing
# 	echo "First handler: ", v
# end
# function a_handler2(v as int) returns nothing
# 	echo "Second handler: ", v
# end
# 
# a_event=geta().event
# a_event.register(a_handler)
# token=a_event.register(a_handler2)
# a_event.fire(3)
# a_event.unregister(token)
# a_event.fire(7)
# 
# ## OUTPUT ##
# First handler: 3
# Second handler: 3
# First handler: 7
