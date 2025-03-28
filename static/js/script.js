// Variables globales
const authModal = document.getElementById('authModal');
const qrModal = document.getElementById('qrModal');
const addBtn = document.getElementById('addQRBtn');
const closeBtns = document.querySelectorAll('.close');

// Mostrar modal de autenticación al hacer clic en el botón
addBtn.addEventListener('click', () => {
    authModal.style.display = 'block';
});

// Cerrar modales
closeBtns.forEach(btn => {
    btn.addEventListener('click', function() {
        this.closest('.modal').style.display = 'none';
    });
});

// Cerrar al hacer clic fuera del modal
window.addEventListener('click', (event) => {
    if (event.target.classList.contains('modal')) {
        event.target.style.display = 'none';
    }
});

// Manejar autenticación
document.getElementById('authForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    const password = document.getElementById('password').value;
    const authMessage = document.getElementById('authMessage');
    
    try {
        const response = await fetch('/verify_admin', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: `password=${encodeURIComponent(password)}`
        });
        
        if (response.ok) {
            authModal.style.display = 'none';
            qrModal.style.display = 'block';
            authMessage.textContent = '';
            document.getElementById('authForm').reset();
        } else {
            const error = await response.json();
            authMessage.textContent = error.error || 'Error de autenticación';
            authMessage.className = 'message error';
        }
    } catch (error) {
        authMessage.textContent = 'Error de conexión';
        authMessage.className = 'message error';
    }
});

// Manejar envío de QR
document.getElementById('qrForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    const qrMessage = document.getElementById('qrMessage');
    qrMessage.textContent = '';
    qrMessage.className = 'message';
    
    const formData = new FormData();
    formData.append('account_name', document.getElementById('account_name').value);
    formData.append('qr_file', document.getElementById('qr_file').files[0]);
    
    try {
        const response = await fetch('/add_account', {
            method: 'POST',
            body: formData
        });
        
        const result = await response.json();
        
        if (response.ok) {
            qrMessage.textContent = `Cuenta ${result.account} agregada con éxito`;
            qrMessage.className = 'message success';
            document.getElementById('qrForm').reset();
            updateTable();
            setTimeout(() => qrModal.style.display = 'none', 2000);
        } else {
            qrMessage.textContent = result.error || 'Error al agregar cuenta';
            qrMessage.className = 'message error';
        }
    } catch (error) {
        qrMessage.textContent = 'Error de conexión';
        qrMessage.className = 'message error';
    }
});

// Función para actualizar la tabla
async function updateTable() {
    try {
        const response = await fetch('/api/totp');
        const data = await response.json();
        const tableBody = document.querySelector('#totpTable tbody');
        
        tableBody.innerHTML = data.map(account => `
            <tr>
                <td>${account.account}</td>
                <td class="code">${account.code}</td>
                <td>
                    <div class="progress-container">
                        <div class="progress-bar" style="width: ${account.percent}%"></div>
                    </div>
                    <span class="time-left">${account.time_left}s</span>
                </td>
            </tr>
        `).join('');
    } catch (error) {
        console.error('Error actualizando tabla:', error);
    }
}

// Variables para el modal de eliminación
const deleteModal = document.getElementById('deleteModal');
const deleteForm = document.getElementById('deleteForm');
const deleteMessage = document.getElementById('deleteMessage');
let currentAccountToDelete = '';

// Función para mostrar modal de eliminación
function showDeleteModal(accountName) {
    currentAccountToDelete = accountName;
    document.getElementById('deleteAccountName').textContent = `¿Eliminar cuenta "${accountName}"?`;
    document.getElementById('delete_account_name').value = accountName;
    deleteModal.style.display = 'block';
}

// Manejar envío del formulario de eliminación
deleteForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    deleteMessage.textContent = '';
    deleteMessage.className = 'message';
    
    const formData = new FormData(deleteForm);
    
    try {
        const response = await fetch('/delete_account', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: new URLSearchParams(formData)
        });
        
        const result = await response.json();
        
        if (response.ok) {
            deleteMessage.textContent = `Cuenta ${result.account} eliminada con éxito`;
            deleteMessage.className = 'message success';
            deleteForm.reset();
            updateTable();
            setTimeout(() => deleteModal.style.display = 'none', 2000);
        } else {
            deleteMessage.textContent = result.error || 'Error al eliminar cuenta';
            deleteMessage.className = 'message error';
        }
    } catch (error) {
        deleteMessage.textContent = 'Error de conexión';
        deleteMessage.className = 'message error';
    }
});

// Modificar la función updateTable para incluir botones de acción
async function updateTable() {
    try {
        const response = await fetch('/api/totp');
        const data = await response.json();
        const tableBody = document.querySelector('#totpTable tbody');
        
        tableBody.innerHTML = data.map(account => `
            <tr>
                <td>${account.account}</td>
                <td class="code">${account.code}</td>
                <td>
                    <div class="progress-container">
                        <div class="progress-bar" style="width: ${account.percent}%"></div>
                    </div>
                    <span class="time-left">${account.time_left}s</span>
                </td>
                <td class="actions">
                    <button onclick="showDeleteModal('${account.account}')" class="delete-btn">Eliminar</button>
                </td>
            </tr>
        `).join('');
    } catch (error) {
        console.error('Error actualizando tabla:', error);
    }
}

// Asegúrate de cerrar el modal de eliminación también
closeBtns.forEach(btn => {
    btn.addEventListener('click', function() {
        this.closest('.modal').style.display = 'none';
    });
});

window.addEventListener('click', (event) => {
    if (event.target.classList.contains('modal')) {
        event.target.style.display = 'none';
    }
});

// Actualizar cada segundo
updateTable();
setInterval(updateTable, 1000);