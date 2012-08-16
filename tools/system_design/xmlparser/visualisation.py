import parser

import numpy as np

from pyx import *
from pyx.connector import arc, curve

def get_y(event):
    """ Get the vertical coordinate for an event line
    
    """
    return 8.5 - (tree.events.keys().index(event.name) * 0.1)

def get_component_width(component):
    """ Get the horizontal width of a component.
     
    """
    text_width = text.text(0, 0, r"\bf %s" % component, textattrs).width

    return max(text_width + 0.1, 0.1 * (len(component.events.subscribe) + len(component.events.publish)+ len(component.actions)))

def get_container_width(container):
    width = 0
    for component in container.components:
        width = width + get_component_width(component) + 0.1
    
    return width
        
def get_graph_width(tree):
    width = 0
    for container in tree.container:
        # skip simulation
        if container.name in ['drive simulation', 'External', 'simulator']:
            continue

        width = width + get_container_width(container) + 0.1
    
    return width    
    

class Visualisation():
    pass

if __name__ == "__main__":
    
    p = parser.Parser()
    p.parse("../../../../roboter/2012_captain/software/global/xml/robot.xml")
    
    tree = p.tree


    unit.set(uscale=7)
    c = canvas.canvas()
    textattrs = [text.halign.center, text.vshift.middlezero]
    texteventattrs = [text.halign.left, text.vshift(-0.6)]
    texteventrotattrs = [text.halign.left, text.vshift(-0.6), trafo.rotate(90), color.rgb.red]
    texteventsubscribedrotattrs = [text.halign.left, text.vshift(-0.6), trafo.rotate(90), color.rgb.blue]
    textactionrotattrs = [text.halign.left, text.vshift(-0.6), trafo.rotate(90), color.rgb(0.0, 0.5, 0.1)]
       
       
       
#    print tree.dump()

    # raster for design
    debug = False
    if debug:
        for x in np.arange(0, 20, 0.05):
            c.stroke(path.line(x, 10, x, 0), 
                   [style.linewidth.thin, color.gray(0.8)])
        
        for y in np.arange(0, 10, 0.05):
            c.stroke(path.line(0, y, 20, y), 
                   [style.linewidth.thin, color.gray(0.8)])
    
        for x in np.arange(0, 20, 0.5):
            c.stroke(path.line(x, 10, x, 0), 
                   [style.linewidth.thin, color.gray(0.5)])
        
        for y in np.arange(0, 10, 0.5):
            c.stroke(path.line(0, y, 20, y), 
                   [style.linewidth.thin, color.gray(0.5)])
        

    # horizontal lines for events
    for event in tree.events:
        event_y = get_y(event)
        c.stroke(path.line(0, event_y, get_graph_width(tree) + 2.1, event_y),
                 [style.linewidth.THick, color.grey(0.5)])
        
        A = text.text(0, event_y, r"\bf [0x%02x] %s" % (event.id, event.name), texteventattrs)
        c.insert(A)
        
        
    # write sourceless and sinkless
    for event in tree.events:
        sourceless = True
        for component in tree.components:
            for event_cmp in component.events.publish:
                if event_cmp == event:
                    sourceless = False
        if sourceless:
            print "event %s is sourceless." % event.name
            A = text.text(1.2, get_y(event), r"\bf sourceless", texteventattrs)
            c.insert(A)

    for event in tree.events:
        sinkless = True
        for component in tree.components:
            for event_cmp in component.events.subscribe:
                if event_cmp.name == event.name:
                    sinkless = False
        if sinkless:
            print "event %s is sinkless." % event.name
            A = text.text(1.5, get_y(event), r"\bf sinkless", texteventattrs)
            c.insert(A)

    container_x = 2.0
    for container in tree.container:
        print container, len(container.components)
        
        # skip simulation
        if container.name in ['drive simulation', 'External', 'simulator']:
            continue
                
        A = text.text(container_x + 0.5, 9.90, r"\bf %s" % container, textattrs)
        
        c.draw(path.rect(container_x + 0.05, 9.7, get_container_width(container), 0.25), 
               [deco.stroked(), deco.filled([color.grey(0.85)])])
        c.insert(A)

        component_x = container_x        
        container_x = container_x + get_container_width(container) + 0.1
        
        
        for component in container.components:
            print "  ", component
                        
            A = text.text(component_x + get_component_width(component)/2 + 0.1, 9.8, r"\bf %s" % component, textattrs)
            
            c.draw(path.rect(component_x + 0.1, 9.75, get_component_width(component), 0.10),
                   [deco.stroked(), deco.filled([color.grey(0.65)])])
            c.insert(A)

            event_x = component_x + 0.15
            component_x = component_x + get_component_width(component) + 0.1
                    
            for event in component.events.publish:
                print "    <--", event
                
                c.stroke(path.line(event_x, 9.75, event_x, get_y(event)),
                         [style.linewidth.THick, color.rgb.red,
                          deco.earrow([deco.stroked([color.rgb.red, style.linejoin.round]),
                                       deco.filled([color.rgb.red])], size=0.05)])
                
                c.fill(path.circle(event_x, 9.75, 0.02), [color.rgb.red])
                
                # write name of event
                
                A = text.text(event_x, 8.6, r"\bf %s" % event.name, texteventrotattrs)
                
                c.insert(A)
                
                event_x = event_x + 0.1
                
            for event in component.events.subscribe:
                print "    -->", event
                
                c.stroke(path.line(event_x, get_y(event), event_x, 9.75),
                         [style.linewidth.THick, color.rgb.blue,
                          deco.earrow([deco.stroked([color.rgb.blue, style.linejoin.round]),
                                       deco.filled([color.rgb.blue])], size=0.05)])

                c.fill(path.circle(event_x, get_y(event), 0.02), [color.rgb.blue])
                
                A = text.text(event_x, 8.6, r"\bf %s" % event.name, texteventsubscribedrotattrs)
                c.insert(A)
               
                event_x = event_x + 0.1
                
            action_x = event_x
            
            for action in component.actions:
                print "  AA-->", action
                
                c.stroke(path.line(action_x, 8.55, action_x, 9.75),
                         [style.linewidth.THick, color.rgb(0.0, 0.5, 0.1),
                          deco.earrow([deco.stroked([color.rgb(0.0, 0.5, 0.1), style.linejoin.round]),
                                       deco.filled([color.rgb(0.0, 0.5, 0.1)])], size=0.05)])
                                
                A = text.text(action_x, 8.6, r"\bf %s" % action.name, textactionrotattrs)
                c.insert(A)
               
                action_x = action_x + 0.1
               
#

    c.writePDFfile("system")
    