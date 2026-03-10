const { contextBridge, ipcRenderer, dialog } = require('electron');

contextBridge.exposeInMainWorld('dialogs', {
  showOpenDialog: () => ipcRenderer.invoke("showOpenDialog"),
  showSaveDialog: () => ipcRenderer.invoke("showSaveDialog"),
});

contextBridge.exposeInMainWorld('server', {
  spawnServerProcess: () => ipcRenderer.invoke("spawnServerProcess"),
})
