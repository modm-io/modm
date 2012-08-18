import parser

import numpy as np
from utils import SingleAssignDictionary

from pyx import *
from pyx.connector import arc, curve

text.set(mode="latex")

# These containers are skipped
skipList = ['drive simulation', 'External', 'simulator']
#skipList = []

def get_y(event):
    """ Get the vertical coordinate for an event line
    
    """
    return 8.5 - (eventsSorted.index(event) * 0.1)

def get_component_width(component):
    """ Get the horizontal width of a component.
     
    """
    text_width = text.text(0, 0, r"\bf %s" % component, textattrs).width

    return max(text_width + 0.1, 0.1 * (len(component.events.subscribe) + len(component.events.publish)+ len(component.actions)))

def get_container_width(container):
    width = 0
    for component in container.components:
        # include actions from abstract component
        component = component.flattened()

        width = width + get_component_width(component) + 0.1
    
    return width
        
def get_graph_width(tree):
    width = 0
    for container in tree.container:
        # skip simulation
        if container.name in skipList:
            continue

        width = width + get_container_width(container) + 0.1
    
    return width    
    

class Visualisation():
    pass

if __name__ == "__main__":
    
    p = parser.Parser()
    
#    file = "../../../../roboter/2012_baba/software/global/xml/robot.xml"
    file = "../../../../roboter/2012_captain/software/global/xml/robot.xml"
    p.parse(file)
    
    tree = p.tree


    unit.set(uscale=7)
    c = canvas.canvas()
    textattrs = [text.halign.center, text.vshift.middlezero]
    texteventattrs = [text.halign.left, text.vshift(-0.6)]
    texteventrotattrs = [text.halign.left, text.vshift(-0.6), trafo.rotate(90), color.rgb.red]
    texteventsubscribedrotattrs = [text.halign.left, text.vshift(-0.6), trafo.rotate(90), color.rgb.blue]
    textactionrotattrs = [text.halign.left, text.vshift(-0.6), trafo.rotate(90), color.rgb(0.0, 0.5, 0.1)]
    
    ### Object to draw
    eventArrowsSource = []
    eventCirclesSource = []
    eventTextSource = []

    eventCirclesSinkless = []
    eventCirclesSourcless = []
    
    eventArrowsSink = []
    eventCirclesSink = []
    eventTextSink = []
    actionArrows = []
    actionText = []
    
       
    sinklessDict = dict()
    sourcelessDict = dict()
       
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
            
    # Sorted events
    # sort by publishing component
    eventsSorted = []
    
    for container in tree.container:
        if container.name in skipList:
            continue
        for component in container.components:
            # include actions from abstract component
            component = component.flattened()
            
            for event in component.events.publish:
                # add only once
                if not (event in eventsSorted):
                    eventsSorted.append(event)
    
    # add Sourceless events to the end
    for event in tree.events:
        if not (event in eventsSorted):
            eventsSorted.append(event)
        
    # horizontal lines for events
    eventsLeft = SingleAssignDictionary('EventLeft')
    eventsRight = dict()
    
    for event in tree.events:
        event_y = get_y(event)
        c.stroke(path.line(0, event_y, get_graph_width(tree) + 2.1, event_y),
                 [style.linewidth.THick, color.grey(0.90)])
        
        A = text.text(0, event_y, r"\bf [0x%02x] %s" % (event.id, event.name), texteventattrs)
        c.insert(A)
        
    # write sourceless and sinkless
    for event in tree.events:
        sourceless = True
        for component in tree.components:
            # include actions from abstract component
            component = component.flattened()

            if component.name in skipList:
                continue
            for event_cmp in component.events.publish:
                if event_cmp == event:
                    sourceless = False
        if sourceless:
            print "event %s is sourceless." % event.name
            A = text.text(1.2, get_y(event), r"\bf sourceless", texteventattrs)
            c.insert(A)
        sourcelessDict[event.name] = sourceless
        
    for event in tree.events:
        sinkless = True
        for component in tree.components:
            # include actions from abstract component
            component = component.flattened()

            if component.name in skipList:
                continue
            for event_cmp in component.events.subscribe:
                if event_cmp.name == event.name:
                    sinkless = False
        if sinkless:
            print "event %s is sinkless." % event.name
            A = text.text(1.5, get_y(event), r"\bf sinkless", texteventattrs)
            c.insert(A)
        sinklessDict[event.name] = sinkless

    # Write whats skipped
    c.insert(text.text(1, 9.8, r"\bf System Design generated from base file", textattrs))
    c.insert(text.text(1, 9.7, r"\bf %s" % file.replace('_', '\_'), textattrs))
    c.insert(text.text(1, 9.5, r"\bf Skipped containers:", textattrs))
    y = 9.40
    for s in skipList:       
        A = text.text(1, y, r"\bf %s" % s, textattrs)
        c.insert(A)
        y = y - .1

    # Draw containers at the top
    container_x = 2.0
    for container in tree.container:
        print container, len(container.components)
        
        # skip simulation
        if container.name in skipList:
            continue
                
        A = text.text(container_x + 0.5, 9.90, r"\bf %s" % container, textattrs)
        
        c.draw(path.rect(container_x + 0.05, 9.65, get_container_width(container), 0.30), 
               [deco.stroked(), deco.filled([color.grey(0.85)])])
        c.insert(A)

        component_x = container_x        
        container_x = container_x + get_container_width(container) + 0.1
        
        
        for component in container.components:
            # include actions from abstract component
            component = component.flattened()
            
            print "  ", component
                        
            A = text.text(component_x + get_component_width(component)/2 + 0.1, 9.8, r"\bf %s" % component, textattrs)
            
            c.draw(path.rect(component_x + 0.1, 9.75, get_component_width(component), 0.10),
                   [deco.stroked(), deco.filled([color.grey(0.65)])])
            c.insert(A)

            event_x = component_x + 0.15
            component_x = component_x + get_component_width(component) + 0.1
            
            # Draw vertical lines for published events        
            for event in component.events.publish:
                print "    <--", event
                
                eventArrowsSource.append(path.line(event_x, 9.75, event_x, get_y(event)))
                eventCirclesSource.append([event_x, 9.75])
                if sinklessDict[event.name]:
                    eventCirclesSinkless.append([event_x, get_y(event)])
                
                # write name of event
                
                A = text.text(event_x, 8.6, r"\bf %s" % event.name, texteventrotattrs)                
                c.insert(A)
                
                # Store most left position
                try:
                    eventsLeft[event.name] = event_x
                except:
                    pass
                
                # this could be the most right position
                eventsRight[event.name] = event_x
                
                event_x = event_x + 0.1
                
            for event in component.events.subscribe:
                print "    -->", event
                
                eventArrowsSink.append(path.line(event_x, get_y(event), event_x, 9.75))
                if sourcelessDict[event.name]:
                    eventCirclesSourcless.append([event_x, get_y(event)])
                else:                
                    eventCirclesSink.append([event_x, get_y(event)])

                
                A = text.text(event_x, 8.6, r"\bf %s" % event.name, texteventsubscribedrotattrs)
                c.insert(A)
               
                # Store most left position
                try:
                    eventsLeft[event.name] = event_x
                except:
                    pass

                eventsRight[event.name] = event_x

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


    ## Now we know from where to where to draw the thick lines
        # horizontal lines for events
#    eventsLeft = SingleAssignDictionary('EventLeft')
#    eventsRight = SingleAssignDictionary('EventRight')
    
    for event in tree.events:
        event_y = get_y(event)
        try:
            event_x_left = eventsLeft[event.name]
            event_x_right = eventsRight[event.name]
            c.stroke(path.line(event_x_left, event_y, event_x_right, event_y),
                     [style.linewidth.THick, color.grey(0.40)])
        except:
            pass
        

    
    ## Draw everything in the right order
    for p in eventArrowsSource:
        c.stroke(p, [style.linewidth.THick, color.rgb.red,
                     deco.earrow([deco.stroked([color.rgb.red, style.linejoin.round]),
                                  deco.filled([color.rgb.red])], size=0.05)])

    for p in eventArrowsSink:
        c.stroke(p, [style.linewidth.THick, color.rgb.blue,
                     deco.earrow([deco.stroked([color.rgb.blue, style.linejoin.round]),
                                  deco.filled([color.rgb.blue])], size=0.05)])
    
    for x, y in eventCirclesSource:
        c.fill(path.circle(x, y, 0.02), [color.rgb.red])
    
    for x, y in eventCirclesSink:
        c.fill(path.circle(x, y, 0.02), [color.rgb.blue])
    
    for x, y in eventCirclesSinkless:
        c.stroke(path.circle(x, y, 0.02), [style.linewidth.THick, color.rgb.red])
 
    for x, y in eventCirclesSourcless:
        c.stroke(path.circle(x, y, 0.02), [style.linewidth.THick, color.rgb.blue])

 
    
    c.writePDFfile("system")
    