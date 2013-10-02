import gdb

class gge_utils_PerformanceTimerPrinter:
	def __init__(self, val):
		self.val=val
		
	def to_string(self):
		return self.val['passed']
	


def lookup_type(val):
     if str(val.type) == 'gge::utils::PerformanceTimer':
        return gge_utils_PerformanceTimerPrinter(val)
	
     return None


gdb.pretty_printers.append(lookup_type)
