#ifndef HTML_CONTENT_H
#define HTML_CONTENT_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Pocket Spectrometer</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #121212;
            color: #ffffff;
            margin: 0;
            padding: 20px;
            text-align: center;
        }
        h1 {
            color: #03DAC6;
            margin-bottom: 30px;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
        }
        .chart-container {
            height: 300px;
            margin: 30px 0;
            position: relative;
            border: 1px solid #333;
            border-radius: 5px;
            padding: 10px;
            background-color: #1e1e1e;
        }
        .bar {
            position: absolute;
            bottom: 40px;
            width: 30px;
            background-color: #BB86FC;
            border-radius: 3px 3px 0 0;
            transition: height 0.3s ease;
        }
        .bar-label {
            position: absolute;
            bottom: 5px;
            width: 30px;
            text-align: center;
            font-size: 10px;
            color: #03DAC6;
        }
        .value-label {
            position: absolute;
            width: 30px;
            text-align: center;
            font-size: 10px;
            color: white;
            top: -20px;
        }
        .info-panel {
            background-color: #1e1e1e;
            border-radius: 5px;
            padding: 15px;
            margin-bottom: 20px;
            text-align: left;
        }
        .info-item {
            margin: 10px 0;
        }
        .info-label {
            color: #03DAC6;
            display: inline-block;
            width: 150px;
        }
        .button {
            background-color: #03DAC6;
            color: #121212;
            border: none;
            padding: 12px 24px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 10px 5px;
            cursor: pointer;
            border-radius: 5px;
            transition: background-color 0.3s;
        }
        .button:hover {
            background-color: #018786;
        }
        .selected-wavelength {
            margin-top: 20px;
            font-size: 18px;
            color: #BB86FC;
        }
        #status {
            margin-top: 10px;
            color: #CF6679;
            height: 20px;
        }
        .save-container {
            margin-top: 20px;
            background-color: #1e1e1e;
            border-radius: 5px;
            padding: 15px;
        }
        .save-container input {
            padding: 10px;
            border-radius: 5px;
            border: 1px solid #333;
            background-color: #2d2d2d;
            color: white;
            margin-right: 10px;
            width: 200px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Pocket Spectrometer</h1>
        
        <div class="info-panel">
            <div class="info-item">
                <span class="info-label">Selected:</span>
                <span id="selected-wavelength">Blue (480nm)</span>
            </div>
            <div class="info-item">
                <span class="info-label">Gain:</span>
                <span id="gain-value">x8</span>
            </div>
            <div class="info-item">
                <span class="info-label">Integration Time:</span>
                <span id="integration-time">100ms</span>
            </div>
        </div>
        
        <div class="chart-container" id="chart">
            <!-- Bars will be added here by JavaScript -->
        </div>
        
        <button class="button" id="measure-btn" onclick="takeMeasurement()">Take Measurement</button>
        <button class="button" id="wavelength-btn" onclick="changeWavelength()">Change Wavelength</button>
        <button class="button" id="gain-btn" onclick="adjustGain()">Adjust Gain</button>
        <button class="button" id="integration-btn" onclick="adjustIntegration()">Adjust Integration</button>
        
        <div class="save-container">
            <button class="button" onclick="downloadData()">Download Data</button>
        </div>
        
        <div id="status"></div>
    </div>

    <script>
        // Initial data
        let spectralData = {
            wavelengths: [415, 445, 480, 515, 555, 590, 630, 680, 940],
            names: ["Violet", "Vio-Blue", "Blue", "Cyan", "Green", "Yellow", "Orange", "Red", "NIR"],
            values: [120, 240, 350, 280, 190, 220, 310, 180, 90],
            gain: 8,
            integration_time: 100,
            selected_index: 2
        };
        
        // Function to update the chart
        function updateChart() {
            const chartContainer = document.getElementById('chart');
            chartContainer.innerHTML = '';
            
            const maxValue = Math.max(...spectralData.values);
            const chartWidth = chartContainer.offsetWidth;
            const barWidth = 30;
            const spacing = (chartWidth - (barWidth * spectralData.wavelengths.length)) / (spectralData.wavelengths.length + 1);
            
            for (let i = 0; i < spectralData.wavelengths.length; i++) {
                const barHeight = (spectralData.values[i] / maxValue) * 220;
                const barLeft = spacing + i * (barWidth + spacing);
                
                // Create bar
                const bar = document.createElement('div');
                bar.className = 'bar';
                bar.style.left = barLeft + 'px';
                bar.style.height = barHeight + 'px';
                
                // Set color based on wavelength
                if (i === spectralData.selected_index) {
                    bar.style.backgroundColor = '#BB86FC';
                    bar.style.boxShadow = '0 0 10px #BB86FC';
                } else {
                    // Approximate colors for each wavelength
                    const colors = [
                        '#8000FF', // Violet
                        '#4B0082', // Vio-Blue
                        '#0000FF', // Blue
                        '#00FFFF', // Cyan
                        '#00FF00', // Green
                        '#FFFF00', // Yellow
                        '#FF8000', // Orange
                        '#FF0000', // Red
                        '#808080'  // NIR (grey)
                    ];
                    bar.style.backgroundColor = colors[i];
                }
                
                // Create wavelength label
                const label = document.createElement('div');
                label.className = 'bar-label';
                label.textContent = spectralData.wavelengths[i];
                label.style.left = barLeft + 'px';
                
                // Create value label
                const valueLabel = document.createElement('div');
                valueLabel.className = 'value-label';
                valueLabel.textContent = spectralData.values[i];
                valueLabel.style.left = barLeft + 'px';
                valueLabel.style.bottom = (barHeight + 5) + 'px';
                
                chartContainer.appendChild(bar);
                chartContainer.appendChild(label);
                chartContainer.appendChild(valueLabel);
            }
            
            // Update info panel
            document.getElementById('selected-wavelength').textContent = 
                spectralData.names[spectralData.selected_index] + ' (' + 
                spectralData.wavelengths[spectralData.selected_index] + 'nm)';
            document.getElementById('gain-value').textContent = 'x' + spectralData.gain;
            document.getElementById('integration-time').textContent = spectralData.integration_time + 'ms';
        }
        
        // Function to fetch new data
        function fetchData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    spectralData = data;
                    updateChart();
                    document.getElementById('status').textContent = '';
                })
                .catch(error => {
                    document.getElementById('status').textContent = 'Error: ' + error;
                });
        }
        
        // Function to take a measurement
        function takeMeasurement() {
            document.getElementById('status').textContent = 'Taking measurement...';
            fetch('/measure')
                .then(response => response.json())
                .then(data => {
                    spectralData = data;
                    updateChart();
                    document.getElementById('status').textContent = 'Measurement complete!';
                    setTimeout(() => {
                        document.getElementById('status').textContent = '';
                    }, 2000);
                })
                .catch(error => {
                    document.getElementById('status').textContent = 'Error: ' + error;
                });
        }
        
        // Function to change selected wavelength
        function changeWavelength() {
            fetch('/change_wavelength')
                .then(response => response.json())
                .then(data => {
                    spectralData.selected_index = data.selected_index;
                    updateChart();
                })
                .catch(error => {
                    document.getElementById('status').textContent = 'Error: ' + error;
                });
        }
        
        // Function to adjust gain
        function adjustGain() {
            fetch('/adjust_gain')
                .then(response => response.json())
                .then(data => {
                    spectralData.gain = data.gain;
                    updateChart();
                })
                .catch(error => {
                    document.getElementById('status').textContent = 'Error: ' + error;
                });
        }
        
        // Function to adjust integration time
        function adjustIntegration() {
            fetch('/adjust_integration')
                .then(response => response.json())
                .then(data => {
                    spectralData.integration_time = data.integration_time;
                    updateChart();
                })
                .catch(error => {
                    document.getElementById('status').textContent = 'Error: ' + error;
                });
        }
        
        // Function to download data to client's device
        function downloadData() {
            // Generate automatic filename with timestamp, wavelength, gain, and integration time
            const now = new Date();
            const timestamp = now.getFullYear() + 
                              ('0' + (now.getMonth() + 1)).slice(-2) + 
                              ('0' + now.getDate()).slice(-2) + '_' +
                              ('0' + now.getHours()).slice(-2) + 
                              ('0' + now.getMinutes()).slice(-2) + 
                              ('0' + now.getSeconds()).slice(-2);
            
            const wavelength = spectralData.wavelengths[spectralData.selected_index];
            const gain = spectralData.gain;
            const integrationTime = spectralData.integration_time;
            
            const filename = `${timestamp}_${wavelength}nm_G${gain}_I${integrationTime}ms.txt`;
            
            // Create text content for the file
            let content = "Pocket Spectrometer Data\n";
            content += "------------------------\n";
            content += "Date: " + now.toLocaleString() + "\n";
            content += "Gain: x" + spectralData.gain + "\n";
            content += "Integration Time: " + spectralData.integration_time + "ms\n";
            content += "Selected Wavelength: " + 
                spectralData.names[spectralData.selected_index] + " (" + 
                spectralData.wavelengths[spectralData.selected_index] + "nm)\n\n";
            
            // Add wavelength headers
            content += "Wavelength (nm),Value\n";
            
            // Add data for each wavelength
            for (let i = 0; i < spectralData.wavelengths.length; i++) {
                content += spectralData.wavelengths[i] + "," + spectralData.values[i] + "\n";
            }
            
            // Create a blob with the data
            const blob = new Blob([content], { type: 'text/plain' });
            
            // Create a download link
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = filename;
            
            // Append to the document, click it, and remove it
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);
            URL.revokeObjectURL(url);
            
            document.getElementById('status').textContent = 'Data downloaded as ' + filename;
            setTimeout(() => {
                document.getElementById('status').textContent = '';
            }, 3000);
        }
        
        // Function to periodically refresh data
        function startAutoRefresh() {
            // Refresh data every 2 seconds
            setInterval(function() {
                fetchData();
            }, 2000);
        }
        // Initialize the chart when the page loads
        window.onload = function() {
            fetchData(); // Get initial data
            startAutoRefresh(); // Start periodic refresh
        };
    </script>
</body>
</html>
)rawliteral";

#endif
