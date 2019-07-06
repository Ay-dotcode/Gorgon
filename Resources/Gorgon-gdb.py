import gdb
import gdb.printing

class geometry_point:
    def __init__(self, val):
        self.val=val
        
    def to_string(self):
        return '(' + str(self.val['X']) + ', ' + str(self.val['Y']) + ')'
    
class geometry_size:
    def __init__(self, val):
        self.val=val
        
    def to_string(self):
        return '(' + str(self.val['Width']) + ', ' + str(self.val['Height']) + ')'
    


def build_pretty_printer():    
    pp = gdb.printing.RegexpCollectionPrettyPrinter("prettygorgon")    
    pp.add_printer('geometry-point', '^Gorgon::Geometry::basic_Point<.*>$', geometry_point)
    pp.add_printer('geometry-size', '^Gorgon::Geometry::basic_Size<.*>$', geometry_size)
    return pp;



gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    build_pretty_printer())
