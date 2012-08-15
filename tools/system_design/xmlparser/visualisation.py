import parser

from pyx import *
from pyx.connector import arc, curve

class Visualisation():
    pass

if __name__ == "__main__":
    
    p = parser.Parser()
    p.parse("../../../../roboter/2012_captain/software/global/xml/robot.xml")
    
    tree = p.tree
    
#    print tree.dump()

    for container in tree.container:
        print container
        
        for component in container.components:
            print "  ", component
            
            for event in component.events.publish:
                print "    <--", event
            
            for event in component.events.subscribe:
                print "    -->", event
