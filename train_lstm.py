import tensorflow as tf
import numpy as np
import pandas as pd
from sklearn.preprocessing import MinMaxScaler

# Load data
df = pd.read_csv('data.csv', names=['analog_voltage', 'avg_analog_voltage', 'pulse_count', 'supply_voltage', 'resistance'])
data = df.values.astype('float32')

# Split data into features and targets
X = data[:, 0:4]
y = data[:, 4]

# Scale features and targets between 0 and 1
scalerX = MinMaxScaler(feature_range=(0, 1))
scalerY = MinMaxScaler(feature_range=(0, 1))
X_scaled = scalerX.fit_transform(X)
y_scaled = scalerY.fit_transform(y.reshape(-1, 1))

# Define LSTM model
model = tf.keras.Sequential()
model.add(tf.keras.layers.LSTM(64, input_shape=(4, 1), return_sequences=True))
model.add(tf.keras.layers.Dropout(0.2))
model.add(tf.keras.layers.LSTM(64, return_sequences=True))
model.add(tf.keras.layers.Dropout(0.2))
model.add(tf.keras.layers.LSTM(64))
model.add(tf.keras.layers.Dropout(0.2))
model.add(tf.keras.layers.Dense(1))
model.compile(loss='mean_squared_error', optimizer='adam')

# Reshape features for LSTM input
X_reshaped = X_scaled.reshape(X.shape[0], 4, 1)

# Train LSTM model
model.fit(X_reshaped, y_scaled, epochs=100, batch_size=32)

# Save trained model
model.save('lstm_model.h5')

# Save feature and target scalers
np.save('scalerX.npy', scalerX.data_max_)
np.save('scalerY.npy', scalerY.data_max_)
