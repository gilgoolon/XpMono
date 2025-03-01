import React, { useState, useEffect } from 'react';
import {
  Container, Box, Typography, Paper, Table, TableBody,
  TableCell, TableContainer, TableHead, TableRow,
  Dialog, DialogTitle, DialogContent, DialogActions,
  Button, TextField, Grid, Card, CardContent,
  IconButton, useMediaQuery
} from '@mui/material';
import { ThemeProvider, createTheme } from '@mui/material/styles';
import CssBaseline from '@mui/material/CssBaseline';
import Brightness4Icon from '@mui/icons-material/Brightness4';
import Brightness7Icon from '@mui/icons-material/Brightness7';
import axios from 'axios';

const API_BASE_URL = 'http://localhost:5000';  // Flask backend URL

function App() {
  const prefersDarkMode = useMediaQuery('(prefers-color-scheme: dark)');
  const [mode, setMode] = useState(prefersDarkMode ? 'dark' : 'light');
  const [clients, setClients] = useState([]);
  const [selectedClient, setSelectedClient] = useState(null);
  const [commandDialog, setCommandDialog] = useState(false);
  const [commandData, setCommandData] = useState('');

  const theme = React.useMemo(
    () =>
      createTheme({
        palette: {
          mode,
          primary: {
            main: mode === 'dark' ? '#90caf9' : '#1976d2',
          },
          secondary: {
            main: mode === 'dark' ? '#f48fb1' : '#dc004e',
          },
          background: {
            default: mode === 'dark' ? '#121212' : '#fff',
            paper: mode === 'dark' ? '#1e1e1e' : '#fff',
          },
        },
      }),
    [mode],
  );

  const toggleColorMode = () => {
    setMode((prevMode) => (prevMode === 'light' ? 'dark' : 'light'));
  };

  useEffect(() => {
    fetchClients();
  }, []);

  const fetchClients = async () => {
    try {
      const response = await axios.get(`${API_BASE_URL}/api/clients`);
      setClients(response.data);
    } catch (error) {
      console.error('Error fetching clients:', error);
    }
  };

  const fetchClientDetails = async (clientId) => {
    try {
      const response = await axios.get(`${API_BASE_URL}/api/clients/${clientId}`);
      setSelectedClient(response.data);
    } catch (error) {
      console.error('Error fetching client details:', error);
    }
  };

  const sendCommand = async () => {
    try {
      await axios.post(`${API_BASE_URL}/api/commands`, {
        client_id: selectedClient.client_id,
        data: commandData
      });
      setCommandDialog(false);
      setCommandData('');
    } catch (error) {
      console.error('Error sending command:', error);
    }
  };

  const formatDate = (dateStr) => {
    return new Date(dateStr).toLocaleString();
  };

  return (
    <ThemeProvider theme={theme}>
      <CssBaseline />
      <Container maxWidth="lg">
        <Box sx={{ my: 4 }}>
          <Box sx={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', mb: 3 }}>
            <Typography variant="h4" component="h1">
              Client Manager
            </Typography>
            <IconButton onClick={toggleColorMode} color="inherit">
              {mode === 'dark' ? <Brightness7Icon /> : <Brightness4Icon />}
            </IconButton>
          </Box>

          <TableContainer component={Paper} sx={{ mb: 4 }}>
            <Table>
              <TableHead>
                <TableRow>
                  <TableCell>Client ID</TableCell>
                  <TableCell>Last Connection</TableCell>
                  <TableCell>Current IP</TableCell>
                  <TableCell>Actions</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {clients.map((client) => (
                  <TableRow key={client.client_id}>
                    <TableCell>{client.client_id}</TableCell>
                    <TableCell>{formatDate(client.last_connection)}</TableCell>
                    <TableCell>{client.current_ip}</TableCell>
                    <TableCell>
                      <Button
                        variant="outlined"
                        onClick={() => fetchClientDetails(client.client_id)}
                        sx={{ mr: 1 }}
                      >
                        Details
                      </Button>
                      <Button
                        variant="contained"
                        onClick={() => {
                          setSelectedClient(client);
                          setCommandDialog(true);
                        }}
                      >
                        Send Command
                      </Button>
                    </TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          </TableContainer>

          {selectedClient && (
            <Card sx={{ mb: 4 }}>
              <CardContent>
                <Typography variant="h6" gutterBottom>
                  Client Details: {selectedClient.client_id}
                </Typography>
                <Grid container spacing={2}>
                  <Grid item xs={12} md={6}>
                    <Typography variant="subtitle1">IP History</Typography>
                    {selectedClient.ip_history.map((entry, index) => (
                      <Typography key={index} variant="body2">
                        {entry.ip} - {formatDate(entry.timestamp)}
                      </Typography>
                    ))}
                  </Grid>
                  <Grid item xs={12} md={6}>
                    <Typography variant="subtitle1">Products</Typography>
                    {selectedClient.products.map((product, index) => (
                      <Typography key={index} variant="body2">
                        {product}
                      </Typography>
                    ))}
                  </Grid>
                </Grid>
              </CardContent>
            </Card>
          )}

          <Dialog open={commandDialog} onClose={() => setCommandDialog(false)}>
            <DialogTitle>Send Command</DialogTitle>
            <DialogContent>
              <TextField
                autoFocus
                margin="dense"
                label="Command Data"
                fullWidth
                multiline
                rows={4}
                value={commandData}
                onChange={(e) => setCommandData(e.target.value)}
                helperText="Enter command in JSON format"
              />
            </DialogContent>
            <DialogActions>
              <Button onClick={() => setCommandDialog(false)}>Cancel</Button>
              <Button onClick={sendCommand} variant="contained">
                Send
              </Button>
            </DialogActions>
          </Dialog>
        </Box>
      </Container>
    </ThemeProvider>
  );
}

export default App;
