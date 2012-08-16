import parser

from pyx import *
from pyx.connector import arc, curve

class Visualisation():
    pass

if __name__ == "__main__":
    
    p = parser.Parser()
    p.parse("../../../../roboter/2012_captain/software/global/xml/robot.xml")
    
    tree = p.tree


    unit.set(uscale=7)
    c = canvas.canvas()
    textattrs = [text.halign.center, text.vshift.middlezero]
    
#    print tree.dump()

    x = 0
    for container in tree.container:
        print container, len(container.components)
        
        A = text.text(x + 0.5, 1.4, r"\bf %s" % container, textattrs)
        
#        c.draw(A.bbox().enlarged(0.1).path(),
#               [deco.stroked(), deco.filled([color.grey(0.85)])])
        c.draw(path.rect(x + 0.05, 1, len(container.components) - 0.1, 0.5), 
               [deco.stroked(), deco.filled([color.grey(0.85)])])
        c.insert(A)
        
        
        for component in container.components:
            print "  ", component
            
            A = text.text(x + 0.5, 1.2, r"\bf %s" % component, textattrs)
            
#            c.draw(A.bbox().enlarged(0.1).path(),
            c.draw(path.rect(x + 0.1, 1.05, 0.8, 0.2),
                   [deco.stroked(), deco.filled([color.grey(0.65)])])
            c.insert(A)

            x = x + 1

            
            
#            for event in component.events.publish:
#                print "    <--", event
#            
#            for event in component.events.subscribe:
#                print "    -->", event


    c.writePDFfile("system")
    