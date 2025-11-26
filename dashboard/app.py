from flask import Flask, render_template
import sqlite3

app = Flask(__name__)

#function to get DB connetion
def get_db_connection():
    conn = sqlite3.connect('plants.db')

    #To help access the columns by name
    conn.row_factory

    return conn

#Page that will display all the plants 
@app.route('/')
def index():
    conn = get_db_connection()
    plants=conn.execute('SELECT DISTINCT plant_id FROM readings').fetchall()
    conn.close()
    return render_template('index.html', plants=plants)

#Route for one specific plant
@app.route('/plant/<plant_id>')
def plant_detail(plant_id):
    conn = get_db_connection()

    readings = conn.execute(
        'SELECT * FROM readings WHERE plant_id = ? ORDER BY timestamp DESC LIMIT 50'
        , (plant_id)).fetchall()
    
    conn.close()
    return render_template('plant.html', plant_id=plant_id, readings=readings)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
    
