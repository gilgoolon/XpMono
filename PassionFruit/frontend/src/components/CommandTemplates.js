import React, { useState, useEffect } from 'react';
import {
  Box,
  Typography,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Button,
  Paper,
  Grid,
  Card,
  CardActionArea,
  Tooltip,
  CircularProgress
} from '@mui/material';
import DescriptionOutlinedIcon from '@mui/icons-material/DescriptionOutlined';
import { API_BASE_URL } from "../Config.js";

export default function CommandTemplates({ onSelectTemplate }) {
  const [templates, setTemplates] = useState([]);
  const [selectedTemplate, setSelectedTemplate] = useState(null);
  const [previewOpen, setPreviewOpen] = useState(false);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchTemplates = async () => {
      try {
        const response = await fetch(`${API_BASE_URL}/api/command-templates`);
        if (!response.ok) {
          throw new Error('Network response was not ok');
        }
        const data = await response.json();
        setTemplates(data);
      } catch (err) {
        setError(err.message);
      } finally {
        setLoading(false);
      }
    };

    fetchTemplates();
  }, []);

  const handlePreview = (template) => {
    setSelectedTemplate(template);
    setPreviewOpen(true);
  };

  const handleUseTemplate = () => {
    if (selectedTemplate) {
      onSelectTemplate(JSON.stringify(selectedTemplate.content, null, 2));
      setPreviewOpen(false);
    }
  };

  if (loading) {
    return (
      <Box sx={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '100%' }}>
        <CircularProgress size={24} />
      </Box>
    );
  }

  if (error) {
    return (
      <Box sx={{ p: 2, textAlign: 'center', color: 'error.main' }}>
        Error: {error}
      </Box>
    );
  }

  return (
    <Box sx={{ height: '100%', display: 'flex', flexDirection: 'column' }}>
      <Typography variant="subtitle2" sx={{ p: 1 }}>
        Command Templates
      </Typography>
      <Paper 
        variant="outlined" 
        sx={{ 
          flex: 1, 
          overflow: 'auto',
          p: 1,
          '&::-webkit-scrollbar': {
            width: '8px',
          },
          '&::-webkit-scrollbar-track': {
            background: (theme) => theme.palette.mode === 'dark' ? '#333' : '#f1f1f1',
          },
          '&::-webkit-scrollbar-thumb': {
            background: (theme) => theme.palette.mode === 'dark' ? '#666' : '#888',
            borderRadius: '4px',
          },
          '&::-webkit-scrollbar-thumb:hover': {
            background: (theme) => theme.palette.mode === 'dark' ? '#888' : '#555',
          },
        }}
      >
        <Grid container spacing={1}>
          {templates.map((template) => (
            <Grid item xs={6} key={template.name}>
              <Tooltip title={template.name} placement="top">
                <Card 
                  variant="outlined"
                  sx={{
                    height: '60px',
                    transition: 'all 0.2s ease-in-out',
                    '&:hover': {
                      transform: 'translateY(-2px)',
                      boxShadow: 2,
                    }
                  }}
                >
                  <CardActionArea 
                    onClick={() => handlePreview(template)}
                    sx={{ 
                      height: '100%',
                      display: 'flex',
                      alignItems: 'center',
                      px: 1,
                    }}
                  >
                    <DescriptionOutlinedIcon sx={{ mr: 1, fontSize: '1.2rem', opacity: 0.7 }} />
                    <Typography 
                      variant="body2" 
                      noWrap 
                      sx={{ 
                        flex: 1,
                        fontWeight: 500,
                      }}
                    >
                      {template.name}
                    </Typography>
                  </CardActionArea>
                </Card>
              </Tooltip>
            </Grid>
          ))}
        </Grid>
      </Paper>

      <Dialog
        open={previewOpen}
        onClose={() => setPreviewOpen(false)}
        maxWidth="md"
        fullWidth
      >
        <DialogTitle>
          {selectedTemplate?.name}
        </DialogTitle>
        <DialogContent>
          <pre style={{ 
            whiteSpace: 'pre-wrap', 
            wordBreak: 'break-word',
            backgroundColor: (theme) => theme.palette.mode === 'dark' ? '#1a1a1a' : '#f5f5f5',
            padding: '16px',
            borderRadius: '4px',
            margin: '8px 0'
          }}>
            {selectedTemplate && JSON.stringify(selectedTemplate.content, null, 2)}
          </pre>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setPreviewOpen(false)}>Cancel</Button>
          <Button onClick={handleUseTemplate} variant="contained">
            Use Template
          </Button>
        </DialogActions>
      </Dialog>
    </Box>
  );
}
