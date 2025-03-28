from flask import Flask, jsonify, render_template, request, session, redirect, url_for
from threading import Thread
import hmac, base64, struct, hashlib, time
import secrets
from werkzeug.utils import secure_filename
import os
from pyzbar.pyzbar import decode
from PIL import Image

app = Flask(__name__, static_folder='static', template_folder='templates')

# Configuración inicial
ACCOUNTS = {
    "UNY STUDY": "H7C2TDQUWRYQVWNI24QEF2CSPW3AJOYC"
}
INTERVAL = 30  # 30 segundos (estándar TOTP)
ADMIN_PASSWORD = "321"  # Contraseña de admin
UPLOAD_FOLDER = 'static/uploads'
ALLOWED_EXTENSIONS = {'png', 'jpg', 'jpeg', 'gif'}

# Configuración de Flask
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
app.config['SECRET_KEY'] = secrets.token_hex(16)
app.config['SESSION_COOKIE_HTTPONLY'] = True
app.config['SESSION_COOKIE_SECURE'] = False  # True en producción con HTTPS

# Estado de las cuentas
account_states = {name: {"code": None, "valid_until": None} for name in ACCOUNTS}

def allowed_file(filename):
    """Verifica si la extensión del archivo es permitida"""
    return '.' in filename and filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

def normalize_secret(secret):
    """Normaliza el secreto para base32"""
    secret = secret.strip().replace(' ', '').upper()
    secret += '=' * ((8 - len(secret) % 8) % 8)  # Padding con =
    return secret

def get_totp_token(secret, interval_length=30):
    """Genera token TOTP estándar"""
    try:
        key = base64.b32decode(normalize_secret(secret), True)
        msg = struct.pack(">Q", int(time.time()) // interval_length)
        h = bytearray(hmac.new(key, msg, hashlib.sha1).digest())
        o = h[19] & 15
        h = str((struct.unpack(">I", h[o:o+4])[0] & 0x7fffffff) % 1000000)
        return h.zfill(6)
    except Exception as e:
        print(f"Error generando token: {e}")
        return "ERROR"

def extract_secret_from_qr(qr_data):
    """Extrae el secreto de los datos del QR"""
    if 'secret=' in qr_data:
        return qr_data.split('secret=')[1].split('&')[0]
    return None

def read_qr_code(filepath):
    """Lee el contenido de un código QR"""
    try:
        img = Image.open(filepath)
        decoded = decode(img)
        return decoded[0].data.decode('utf-8') if decoded else None
    except Exception as e:
        print(f"Error leyendo QR: {e}")
        return None

def update_codes():
    """Actualiza los códigos periódicamente"""
    while True:
        for name, secret in ACCOUNTS.items():
            account_states[name]["code"] = get_totp_token(secret)
            account_states[name]["valid_until"] = (int(time.time()) // INTERVAL + 1) * INTERVAL
        time.sleep(INTERVAL - (time.time() % INTERVAL))

@app.route('/verify_admin', methods=['POST'])
def verify_admin():
    """Verifica la contraseña de admin"""
    if request.form.get('password') == ADMIN_PASSWORD:
        session['admin_authenticated'] = True
        return jsonify({'success': True})
    return jsonify({'error': 'Contraseña incorrecta'}), 401

@app.route('/add_account', methods=['POST'])
def add_account():
    """Agrega nueva cuenta desde QR"""
    if not session.get('admin_authenticated'):
        return jsonify({'error': 'No autorizado'}), 403
    
    if 'qr_file' not in request.files:
        return jsonify({'error': 'No se seleccionó archivo'}), 400
    
    file = request.files['qr_file']
    account_name = request.form.get('account_name', '').strip()
    
    if not (file and file.filename and account_name):
        return jsonify({'error': 'Datos incompletos'}), 400
    
    if not allowed_file(file.filename):
        return jsonify({'error': 'Tipo de archivo no permitido'}), 400
    
    try:
        # Guardar archivo temporalmente
        filename = secure_filename(file.filename)
        filepath = os.path.join(app.config['UPLOAD_FOLDER'], filename)
        os.makedirs(os.path.dirname(filepath), exist_ok=True)
        file.save(filepath)
        
        # Leer QR y extraer secreto
        qr_data = read_qr_code(filepath)
        secret = extract_secret_from_qr(qr_data) if qr_data else None
        
        if not secret:
            return jsonify({'error': 'No se pudo extraer el secreto del QR'}), 400
        
        # Agregar nueva cuenta
        ACCOUNTS[account_name] = secret
        account_states[account_name] = {"code": None, "valid_until": None}
        
        return jsonify({
            'success': True,
            'account': account_name,
            'code': get_totp_token(secret)
        })
    except Exception as e:
        return jsonify({'error': str(e)}), 500
    finally:
        # Limpiar archivo temporal
        if 'filepath' in locals() and os.path.exists(filepath):
            os.remove(filepath)

@app.route('/api/totp')
def get_all_totp():
    """Obtiene los datos de todas las cuentas"""
    current_time = int(time.time())
    accounts_data = []
    
    for name, state in account_states.items():
        valid_until = state["valid_until"] or (current_time // INTERVAL + 1) * INTERVAL
        time_left = max(0, valid_until - current_time)
        
        accounts_data.append({
            "account": name,
            "code": state["code"] or get_totp_token(ACCOUNTS[name]),
            "time_left": int(time_left),
            "percent": int((time_left / INTERVAL) * 100)
        })
    
    return jsonify(accounts_data)

@app.route('/')
def index():
    """Página principal"""
    return render_template('index.html')
@app.route('/delete_account', methods=['POST'])
def delete_account():
    """Elimina una cuenta existente"""
    if not session.get('admin_authenticated'):
        return jsonify({'error': 'No autorizado'}), 403
    
    account_name = request.form.get('account_name')
    if not account_name:
        return jsonify({'error': 'Nombre de cuenta requerido'}), 400
    
    if account_name not in ACCOUNTS:
        return jsonify({'error': 'Cuenta no encontrada'}), 404
    
    # Eliminar la cuenta
    del ACCOUNTS[account_name]
    del account_states[account_name]
    
    return jsonify({'success': True, 'account': account_name})

if __name__ == '__main__':
    # Crear carpeta de uploads si no existe
    os.makedirs(UPLOAD_FOLDER, exist_ok=True)
    
    # Iniciar hilo para actualizar códigos
    Thread(target=update_codes, daemon=True).start()
    
    # Iniciar servidor
    app.run(host='0.0.0.0', port=5000, debug=True)