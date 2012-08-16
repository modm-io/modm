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

    # horizontal lines for events
    for event in tree.events:
        event_y = 0.0 - (event.id * 0.1)
        c.stroke(path.line(0, event_y, 25, event_y),
                 [style.linewidth.THICK, color.grey(0.1)])

    x = 0
    for container in tree.container:
        print container, len(container.components)
        
        A = text.text(x + 0.5, 1.4, r"\bf %s" % container, textattrs)
        
        c.draw(path.rect(x + 0.05, 1, len(container.components) - 0.1, 0.5), 
               [deco.stroked(), deco.filled([color.grey(0.85)])])
        c.insert(A)
        
        
        for component in container.components:
            print "  ", component
            
            A = text.text(x + 0.5, 1.2, r"\bf %s" % component, textattrs)
            
            c.draw(path.rect(x + 0.1, 1.05, 0.8, 0.2),
                   [deco.stroked(), deco.filled([color.grey(0.65)])])
            c.insert(A)

            event_x = x + 0.15
            x = x + 1
                    
            for event in component.events.publish:
                print "    <--", event
                
                c.stroke(path.line(event_x, 1.1, event_x, 0.0 - (event.id * 0.1)),
                         [style.linewidth.THICK, color.rgb.red,
                          deco.earrow([deco.stroked([color.rgb.red, style.linejoin.round]),
                                       deco.filled([color.rgb.red])], size=0.05)])
                
                event_x = event_x + 0.1
                
            for event in component.events.subscribe:
                print "    -->", event
                
                c.stroke(path.line(event_x, 0.0 - (event.id * 0.1), event_x, 1.1),
                         [style.linewidth.THICK, color.rgb.blue,
                          deco.earrow([deco.stroked([color.rgb.blue, style.linejoin.round]),
                                       deco.filled([color.rgb.blue])], size=0.05)])
                
                event_x = event_x + 0.1



    c.writePDFfile("system")
    