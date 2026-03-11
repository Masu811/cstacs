const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('api', {
  showOpenDialog: () => ipcRenderer.invoke("showOpenDialog"),
  showSaveDialog: () => ipcRenderer.invoke("showSaveDialog"),
  health: () => ipcRenderer.invoke("health"),
});
