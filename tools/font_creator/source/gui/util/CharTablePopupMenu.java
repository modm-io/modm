/*
 * Package:    gui.util
 * File:       CharTablePopupMenu.java
 * Created on: 14.01.2005
 *
 */
package gui.util;

import gui.CharTable;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

//import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;

/**
 * @author Ape
 */
public class CharTablePopupMenu extends JPopupMenu implements ActionListener {
	private static final long serialVersionUID = -6618286029163294898L;
	//private CharTable parent;

	public CharTablePopupMenu(CharTable parent) {
		//this.parent = parent;

		/*JMenuItem insert = new JMenuItem("Insert Char");
		insert.setActionCommand("insert");
		insert.addActionListener(this);
		this.add(insert);

		JMenuItem remove = new JMenuItem("Remove Char");
		remove.setActionCommand("remove");
		remove.addActionListener(this);
		this.add(remove);*/
	}

	public void actionPerformed(ActionEvent e) {
		/*String cmd = e.getActionCommand();
		if (cmd.equals("insert")) {
			parent.insertNewCharacter();
		} else if (cmd.equals("remove")) {
			parent.removeSelectedCharacter();
		}*/
	}

}
