import gdb
import gdb.printing
from pprint import pprint

class geometry_point:
    def __init__(self, val):
        self.val=val
        
    def to_string(self):
        return '(' + str(self.val['X']) + ', ' + str(self.val['Y']) + ')'

class geometry_size:
    def __init__(self, val):
        self.val=val
        
    def to_string(self):
        return '(' + str(self.val['Width']) + ' x ' + str(self.val['Height']) + ')'
    

class geometry_bounds:
    def __init__(self, val):
        self.val=val
        
    def to_string(self):
        return '<(' + str(self.val['Left']) + ', ' + str(self.val['Top']) + ') - (' + str(self.val['Right']) + ', ' + str(self.val['Bottom']) + ')]'
    

class geometry_margin:
    def __init__(self, val):
        self.val=val
        
    def to_string(self):
        return '(' + str(self.val['Left']) + ', ' + str(self.val['Top']) + ', ' + str(self.val['Right']) + ', ' + str(self.val['Bottom']) + ')'
    

class geometry_rect:
    def __init__(self, val):
        self.val=val
        
    def to_string(self):
        return '<(' + str(self.val['Left']) + ', ' + str(self.val['Top']) + '), ' + str(self.val['Width']) + ' x ' + str(self.val['Height']) + '>'

class ui_dim:
    def __init__(self, val):
        self.val=val
        
    def to_string(self):
        s = str(self.val['value'])
            
        print(str(s)+'!')
        
        if(self.val['unit'] == 0):
            s = str(s) + 'px'
        elif(self.val['unit'] == 1):
            s = str(s) + '%'
        elif(self.val['unit'] == 2):
            s = str(s) + 'mpx'
        elif(self.val['unit'] == 3):
            s = str(s) + 'bp'
        elif(self.val['unit'] == 4):
            s = str(s) + 'em'
        elif(self.val['unit'] == 5):
            s = str(s) + 'units'
        elif(self.val['unit'] == 6):
            s = str(s) + 'munits'
        elif(self.val['unit'] == 7):
            s = str(s) + 'spcs'
        elif(self.val['unit'] == 8):
            s = str(s) + 'fr'
        else:
            s = str(s) + str(self.val['unit'])
            
        return s

def build_pretty_printer():    
    pp = gdb.printing.RegexpCollectionPrettyPrinter("prettygorgon")    
    pp.add_printer('geometry-point', '^Gorgon::Geometry::basic_Point<.*>$', geometry_point)
    pp.add_printer('geometry-size', '^Gorgon::Geometry::basic_Size<.*>$', geometry_size)
    pp.add_printer('geometry-bounds', '^Gorgon::Geometry::basic_Bounds<.*>$', geometry_bounds)
    pp.add_printer('geometry-margin', '^Gorgon::Geometry::basic_Margin<.*>$', geometry_margin)
    pp.add_printer('geometry-rect', '^Gorgon::Geometry::basic_Rectangle<.*>$', geometry_rect)
    pp.add_printer('ui-dimension', '^Gorgon::UI::(Unit)?Dimension$', ui_dim)
    return pp;



gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    build_pretty_printer())
