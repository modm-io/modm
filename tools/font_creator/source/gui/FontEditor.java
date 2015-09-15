
package gui;

import java.awt.BorderLayout;
import java.awt.Dimension;

import javax.swing.JInternalFrame;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTable;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameListener;

import util.Font;
import util.ImageTransferHandler;

/**
 * Font editor window.
 * 
 * Consits of a list of all characters (CharTable) and an editor for a
 * single Character (CharEditor).
 * 
 * @author Fabian Maximilian Thiele
 */
public class FontEditor extends JInternalFrame implements InternalFrameListener {
	private static final long serialVersionUID = -6061235742858518585L;
	private JScrollPane editorScrollPane;
	private CharEditor charEditor;
	private CharTable charTable;
	private JSplitPane splitPane;
	private Font font;
	private MainWindow mainWindow;
	private String filename;
	
	/**
	 * 
	 * @param font
	 * @param mainWindow
	 * @param filename		Filename from which the font was loaded
	 */
	public FontEditor(Font font, MainWindow mainWindow, String filename) {
		this.font = font;
		this.mainWindow = mainWindow;
		this.filename = filename;
		initGUI();
		initInternalFrame();
		addInternalFrameListener(this);
	}
	
	private void initGUI() {
		this.setLayout(new BorderLayout());
		
		charEditor = new CharEditor(font.getCharacter(0), this,
				font.getEditorColorModel(), mainWindow);
		charEditor.setChar(font.getCharacter(0));
		charEditor.setTransferHandler(new ImageTransferHandler());
		editorScrollPane = new JScrollPane(charEditor);
		editorScrollPane.setPreferredSize(new Dimension(450, 450));

		charTable = new CharTable(font, this);
		JTable table = charTable.getTable();
		JScrollPane sp2 = new JScrollPane(table);
		sp2.setPreferredSize(new Dimension(232, 100));
		sp2.setMinimumSize(new Dimension(232, 100));

		splitPane = new JSplitPane();
		splitPane.setLeftComponent(editorScrollPane);
		splitPane.setRightComponent(sp2);
		splitPane.setResizeWeight(1.0);
		splitPane.setOneTouchExpandable(true);

		this.add(BorderLayout.CENTER, splitPane);
	}

	private void initInternalFrame() {
		pack();
		setTitle(font.getName());
		setIconifiable(true);
		setMaximizable(true);
		setClosable(true);
		setResizable(true);
	}

	public Font getEditedFont() {
		return font;
	}
	
	public String getFilename() {
		return filename;
	}
	
	public void setFilename(String filename) {
		this.filename = filename;
	}
	
	public void setEditingChar(int index) {
		charEditor.setChar(font.getCharacter(index));
		editingCharSizeChanged();
	}

	public void setZoom(int zoom) {
		charEditor.setZoom(zoom);
		editingCharSizeChanged();
	}

	public void editingCharSizeChanged() {
		editorScrollPane = new JScrollPane(charEditor);
		splitPane.setLeftComponent(editorScrollPane);
	}

	public void internalFrameOpened(InternalFrameEvent arg0) {
		// unused
	}

	public void internalFrameClosing(InternalFrameEvent arg0) {
		// unused
	}

	public void internalFrameClosed(InternalFrameEvent arg0) {
		// unused
	}

	public void internalFrameIconified(InternalFrameEvent arg0) {
		// unused
	}

	public void internalFrameDeiconified(InternalFrameEvent arg0) {
		// unused
	}
	
	public void internalFrameActivated(InternalFrameEvent arg0) {
		mainWindow.selectedEditorChanged(charEditor);
	}
	
	public void internalFrameDeactivated(InternalFrameEvent arg0) {
		// unused
	}
}
