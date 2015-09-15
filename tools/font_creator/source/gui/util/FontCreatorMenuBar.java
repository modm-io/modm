/*
 * Created on 11.02.2004
 *
 * To change the template for this generated file go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation&gt;Code and Comments
 */
package gui.util;

import gui.MainWindow;

import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.image.BufferedImage;
import java.io.IOException;

import javax.swing.Action;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;
import javax.swing.TransferHandler;
import javax.swing.text.DefaultEditorKit;

import util.TransferActionListener;

/**
 * @author Fabian Maximilian Thiele
 */
public class FontCreatorMenuBar extends JMenuBar implements ActionListener {
	private static final long serialVersionUID = -1128462201083834057L;
	private MainWindow gui;

	public FontCreatorMenuBar(MainWindow gui) {
		this.gui = gui;
		initMenuBar();
	}

	private void initMenuBar() {
		JMenu menu = new JMenu("File");

		JMenuItem newFont = new JMenuItem("New Font");
		newFont.setActionCommand("newFont");
		newFont.addActionListener(this);
		menu.add(newFont);

		JMenuItem loadFont = new JMenuItem("Open Font");
		loadFont.setActionCommand("loadFont");
		loadFont.addActionListener(this);
		menu.add(loadFont);

		JMenuItem saveFont = new JMenuItem("Save Font");
		saveFont.setActionCommand("saveFont");
		saveFont.addActionListener(this);
		menu.add(saveFont);

		menu.addSeparator();

		JMenuItem exit = new JMenuItem("Exit");
		exit.setActionCommand("exit");
		exit.setMnemonic(KeyEvent.VK_E);
		exit.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F4,
				ActionEvent.ALT_MASK));
		exit.addActionListener(this);
		menu.add(exit);

		add(menu);

		JMenu edit = new JMenu("Edit");

		JMenuItem fontSettings = new JMenuItem("Edit Font Settings");
		fontSettings.setActionCommand("fontSettings");
		fontSettings.addActionListener(this);
		edit.add(fontSettings);

		TransferActionListener al = new TransferActionListener();

		JMenuItem copy = new JMenuItem(new DefaultEditorKit.CopyAction());
		copy.setText("Copy");
		copy.setMnemonic(KeyEvent.VK_C);
		copy.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C,
				ActionEvent.CTRL_MASK));
		copy.setActionCommand((String) TransferHandler.getCopyAction().getValue(
				Action.NAME));
		copy.addActionListener(al);
		edit.add(copy);

		JMenuItem paste = new JMenuItem(new DefaultEditorKit.PasteAction());
		paste.setText("Paste");
		paste.setMnemonic(KeyEvent.VK_P);
		paste.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_V,
				ActionEvent.CTRL_MASK));
		paste.setActionCommand((String) TransferHandler.getPasteAction().getValue(
				Action.NAME));
		paste.addActionListener(al);
		edit.add(paste);

		add(edit);

		JMenu export = new JMenu("Export");

		JMenuItem createHeader = new JMenuItem("Export Font");
		createHeader.setActionCommand("export");
		createHeader.addActionListener(this);
		export.add(createHeader);

		add(export);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	public void actionPerformed(ActionEvent e) {
		String cmd = e.getActionCommand();
		if (cmd.equals("newFont")) {
			gui.newFont();
		} else if (cmd.equals("loadFont")) {
			gui.loadFont();
		} else if (cmd.equals("saveFont")) {
			gui.saveFont();
		} else if (cmd.equals("exit")) {
			System.exit(0);
		} else if (cmd.equals("export")) {
			gui.exportFont();
		} else if (cmd.equals("fontSettings")) {
			gui.editFontSettings();
		} else if (cmd.equals("paste")) {
			Toolkit tk = Toolkit.getDefaultToolkit();
			Clipboard cb = tk.getSystemClipboard();
			Transferable trans = cb.getContents(null);
			System.out.println("Clipboard:    " + cb.getName());
			System.out.println("Transferable: " + trans);
			DataFlavor cbData = (trans.getTransferDataFlavors())[0];
			System.out.println(0 + ": " + cbData.getHumanPresentableName());
			try {
				BufferedImage img = (BufferedImage) trans.getTransferData(cbData);
				System.out.println(img);
			} catch (UnsupportedFlavorException ex) {
				ex.printStackTrace();
			} catch (IOException ex) {
				ex.printStackTrace();
			} catch (ClassCastException ex) {
				ex.printStackTrace();
			}
		}
	}

}
